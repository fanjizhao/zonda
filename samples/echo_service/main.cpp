#include "rpc/RpcEventHandlerBase.h"
#include "seda/Stage.h"
#include "rpc/RpcService.h"
#include "log/Logger.h"
#include "rpc/ServiceStageMap.h"
#include "rpc/RouteTable.h"
#include "inc/MsgFactory.h"
#include "thread/ThreadMgr.h"
#include "util/TimerService.h"
#include "util/TimerEvent.h"
#include "inc/services.h"
#include "util/MsgPool.h"
#include "util/FsmMgr.h"
//#include "util/Configurator.h"
#include "log/Logger.h"
#include "config/Config.h"
#include "util/GlobalTimer.h"

using namespace zonda;
using namespace zonda::common;
using namespace zonda::msg::echo_service;
using namespace zonda::msg::seq_service;


class EchoFsm: public FsmBase<EchoFsm>
{
public:
    RpcHead* rpc_head;
    EchoExReq* req;
    void reset()
    {
        rpc_head = 0;
        req = NULL;
    }
};


struct BizHandlerParam
{
    zonda::common::RpcService* rpc;

    int echo_fsm_count;    
};


class BizHandler: public zonda::common::RpcEventHandlerBase
{
public:
    BizHandler();
    virtual int init(zonda::common::IStage* stage, void* param);
    virtual int handle_event(const zonda::common::IEvent* event);
    virtual ~BizHandler() {};
    
    virtual int handle_rpc_req(
        int op_type, 
        IMsg* msg, 
        RpcHead* rpc_head);
    
    //! When an rpc response comes back, the method is called   
    virtual int handle_rpc_res(int op_type,
            int res_code,
            IMsg *msg,
            int64_t trans_param,
            int64_t transaction_id,
            IMsg* original_req);
private:
    zonda::common::RpcService* m_rpc;
    zonda::common::IStage* m_self_stage;
    FsmMgr<EchoFsm> m_fsm_mgr;
    int send_times;
    int recv;
   
};


BizHandler::BizHandler():
    m_rpc(NULL)
{
    send_times = 0;
    recv = 0;
}

int BizHandler::init(zonda::common::IStage* stage, void* param)
{
    m_self_stage = stage;
    BizHandlerParam* p = (BizHandlerParam*)param;
    m_rpc = p->rpc;
    
    int r = m_fsm_mgr.init(p->echo_fsm_count);
    
    if (r != 0)
    {
        LOG_FATAL("Failed to init fsm mgr");
        return -1;
    }
    /*    
    r = m_rpc->set_stage_service(m_self_stage->get_sink(), zonda::service_type::CLIENT);
    if (r != 0)
    {
        LOG_FATAL("Failed to call set_stage_service for CLIENT");
        return -1;
    }*/
    

    
    return 0;
}

int BizHandler::handle_event(const zonda::common::IEvent* event)
{
    int event_type = event->get_type();
    
    if (event_type == zonda::common::EventType::RPC_EVENT)
    {
        return handle_rpc_event(event);
    }
    
    if (event_type == zonda::common::EventType::TIMER_EVENT)
    {
        zonda::common::TimerEvent* e = (zonda::common::TimerEvent*)event;
        LOG_ERROR("recv=" << recv << ", send_times = " << send_times);
    }
    delete event;
    return 0;
}

int BizHandler::handle_rpc_req(int op_type, zonda::common::IMsg* msg, RpcHead* rpc_head)
{
    int64_t call_id = rpc_head->call_id; 
    int64_t trans_param = rpc_head->trans_param;
    int64_t transaction_id = rpc_head->transaction_id;

    LOG_DEBUG("Received msg, type:" << msg->get_msg_type()
        << ", call_id:" << call_id 
        << ", trans_param:" << trans_param
        << ", transaction_id:" << transaction_id);
    
    if (msg->get_msg_type() == zonda::msg_type::echo_service::ECHO_EX_REQ)
    {
        LOG_DEBUG("Received EchoExReq, call_id:" << call_id 
            << ", trans_param:" << trans_param
            << ", transaction_id:" << transaction_id);
        EchoExReq* req = (EchoExReq*)msg;    
        
        //TODO: Save the necessary data into an FSM
        //Send the res with trans_param = fsm_id
        EchoFsm* fsm = m_fsm_mgr.create_fsm();
        fsm->rpc_head = rpc_head;
        fsm->req = req;
        
        GetSeqReq* seq_req = GetSeqReq::create();
        seq_req->seq_type = 0;
        seq_req->count = 1;
        ServiceAddr seq_serv_addr;
        seq_serv_addr.service_type = service_type::SEQ_SERVICE;
        seq_serv_addr.group_id = 0;
        seq_serv_addr.role = 1;
        
        LOG_DEBUG("Send GetSeqReq to get the times, fsm_id:" << fsm->get_id()
            << ", transaction_id:" << transaction_id);
        int r = m_rpc->send_req(m_self_stage, seq_req, 
            transaction_id, fsm->get_id(), seq_serv_addr);
        
        if (r != 0)
        {
            LOG_ERROR("Failed to send GetSeqReq, fsm_id:" << fsm->get_id()
                << ", transaction_id:" << transaction_id);
            req->free();
            seq_req->free();
            fsm->free();
        }        
        
    }
    else if (msg->get_msg_type() == zonda::msg_type::echo_service::ECHO_REQ)
    {
        ++recv;
        EchoReq* req = (EchoReq*)msg;
        EchoRes* res = EchoRes::create();
        res->str.swap(req->str);
        int r = m_rpc->send_res(rpc_head, 0, res);
        if (r != 0)
        {
            LOG_ERROR("Failed to send EchoRes, str:" << res->str);
            res->free();
        }
        msg->free();
        ++send_times;
        /*
        if (send_times == 10000)
        {
            sleep(10);
            exit(0);
            //m_rpc->output_stat();
        } */       
    }
    return 0;
}        

int BizHandler::handle_rpc_res(
            int op_type,
            int res_code,
            zonda::common::IMsg *msg,
            int64_t trans_param,
            int64_t transaction_id,
            IMsg* original_req)
{
    LOG_DEBUG("Received res, op_type:" << op_type
        << ", res_code:" << res_code 
        << ", trans_param:" << trans_param
        << ", transaction_id:" << transaction_id);
    
    if (op_type == zonda::op_type::echo_service::ECHO_EX)
    {
        if (res_code == 0)
        {
            EchoExRes* res = (EchoExRes*)msg;
            LOG_DEBUG("Received, EchoExRes, result:" << res->result
                << ", times:" << res->times
                << ", ret_code: " << res->ret_code
                << ", trans_param:" << trans_param
                << ", transaction_id:" << transaction_id);
            //LOG_DEBUG("msg has been destroyed successfully");
            msg->free();
        }
        else
        {
            LOG_DEBUG("Failed to recv EchoExRes, transaction_id:"
                << transaction_id << ", trans_param:" << trans_param);
        }
    }
    else if (op_type == op_type::seq_service::GET_SEQ)
    {
        EchoFsm* fsm = NULL;
        if (res_code == 0)
        {
            GetSeqRes* res = (GetSeqRes*)msg;
            LOG_DEBUG("Received GetSeqRes, start:" << res->start
                << ", count:" << res->count
                << ", trans_param:" << trans_param
                << ", transaction_id:" << transaction_id);
                        
            
            EchoExRes* echo_res = EchoExRes::create();
            echo_res->times = res->start;
            
           
            fsm = m_fsm_mgr.get_fsm(trans_param);
            echo_res->result = fsm->req->str;
            
            int r = 0;
            LOG_DEBUG("Send EchoExRes, call_id:" << fsm->rpc_head->call_id
                << ", transaction_id:" << transaction_id);
                 
            r = m_rpc->send_res(fsm->rpc_head, 0, echo_res);
            if (r != 0)
            {
                LOG_ERROR("Failed to send EchoExRes, call_id:" << fsm->rpc_head->call_id
                    << ", transaction_id:" << transaction_id);            
                echo_res->free();
            }        
            
            res->free();
        }
        else
        {
            LOG_DEBUG("Received error GetSeqRes, trans_param:" << trans_param
                << ", transaction_id:" << transaction_id);
            EchoExRes* echo_res = EchoExRes::create();
            echo_res->ret_code = -1;
            
           
            fsm = m_fsm_mgr.get_fsm(trans_param);
            echo_res->result = fsm->req->str;
            
            int r = 0;
            LOG_DEBUG("Send error EchoExRes, call_id:" << fsm->rpc_head->call_id
                << ", transaction_id:" << transaction_id);
            r = m_rpc->send_res(fsm->rpc_head, 0, echo_res);
            if (r != 0)
            {
                LOG_ERROR("Failed to send res");            
                echo_res->free();
            }
        }
        fsm->req->free();
        fsm->free();
    }
            
    return 0;
}            

int main()
{
    int r = 0;
    zonda::logger::LoggerFactory::init("echo_service", "echo_service.conf", false);

    //r = Configurator::instance()->load("echo_service.conf");
    //RouteTable::instance()->load("route.conf");
    
    LOG_DEBUG("I am echo_service");
    r = Config::instance()->init(NULL);
    if (r != 0)
    {
        LOG_FATAL("Failed to init config");
        return 0;
    }
      
    zonda::MsgFactory msg_factory;
    zonda::common::RpcService my_rpc;
    zonda::common::RpcServiceParam rpc_param(&msg_factory);
    //rpc_param.ip = "10.20.153.132";
    //rpc_param.port = 8888;
    
    r = my_rpc.init(&rpc_param);
    if (r != 0)
    {
        LOG_FATAL("Failed to init my_rpc");
        return 0;        
    }
    
    r = my_rpc.start();
    if (r != 0)
    {
        LOG_FATAL("Failed to start my_rpc");
        return 0;            
    }   
    
    zonda::common::Stage<BizHandler> biz_stage("biz_stage", 100000, 2);
    BizHandlerParam param;
    param.rpc = &my_rpc;

    param.echo_fsm_count = 10000;
    
    r = biz_stage.init(&param);
    if (r != 0)
    {
        LOG_FATAL("Failed to init biz_stage");
        return 0;           
    }
    
    r = biz_stage.start();
    if (r != 0)
    {
        LOG_FATAL("Failed to start biz_stage");
        return 0;           
    }
    
    r = my_rpc.set_stage_service(&biz_stage, zonda::service_type::ECHO_SERVICE);
    if (r != 0)
    {
        LOG_FATAL("Failed to call set_stage_service for ECHO_SERVICE");
        return -1;
    }    
    //GlobalTimer::instance()->init();
    //GlobalTimer::instance()->schedule(biz_stage.get_sink(), 1, 10000, 10000);
    zonda::common::ThreadMgr::instance()->wait_all();   
}
