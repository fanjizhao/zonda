#include "inc/ServiceBase.h"
#include "rpc/MasterSlaveSwitchEvent.h"

using namespace zonda;
using namespace zonda::common;
using namespace zonda::msg::seq_service;

struct BizHandlerParam
{
    zonda::common::IRpcService* rpc;
    std::string host;
    uint16_t port;      
};


class BizHandler: public zonda::common::RpcEventHandlerBase
{
public:
    BizHandler();
    virtual int init(zonda::common::IStage* stage, void* param);
    virtual int handle_event(const zonda::common::IEvent* event);
    virtual ~BizHandler() {};
    
    virtual int handle_rpc_req(int op_type, zonda::common::IMsg* msg, 
        RpcHead* rpc_head);
    
    //! When an rpc response comes back, the method is called   
    virtual int handle_rpc_res(
            int op_type,
            int res_code,
            zonda::common::IMsg *msg,
            int64_t trans_parm,
            int64_t transaction_id,
            IMsg* original_req);
private:
    zonda::common::IRpcService* m_rpc;
    zonda::common::IStage* m_self_stage;
    std::map<int, int64_t>  m_seq_map;
    int send_times;          
};


BizHandler::BizHandler():
    m_rpc(NULL)
{
    send_times = 0;
}

int BizHandler::init(zonda::common::IStage* stage, void* param)
{
    m_self_stage = stage;
    m_rpc = (zonda::common::IRpcService*)param;
    return 0;
}

int BizHandler::handle_event(const zonda::common::IEvent* event)
{
    int event_type = event->get_type();
    
    if (event_type == zonda::common::EventType::RPC_EVENT)
    {
        return handle_rpc_event(event);
    }
    
    if (event_type == EventType::MASTER_SLAVE_SWITCH_EVENT)
    {
        MasterSlaveSwitchEvent* e = (MasterSlaveSwitchEvent*)event;
        LOG_DEBUG("Received MasterSlaveSwitchEvent, service_type:"
            << e->service_type
            << ", old_role:" << ServiceInstanceRole::get_desc(e->old_role)
            << ", new_role:" << ServiceInstanceRole::get_desc(e->new_role));
    }

    delete event;
    return 0;
}

int BizHandler::handle_rpc_req(int op_type, zonda::common::IMsg* msg, 
    RpcHead* rpc_head)
{
    
    int64_t call_id = rpc_head->call_id; 
    int64_t trans_param = rpc_head->trans_param;
    int64_t transaction_id = rpc_head->transaction_id;
    
    LOG_DEBUG("Received msg, type:" << msg->get_msg_type()
        << ", call_id:" << call_id 
        << ", trans_param:" << trans_param
        << ", transaction_id:" << transaction_id);
    
    if (msg->get_msg_type() == msg_type::seq_service::GET_SEQ_REQ)
    {
        GetSeqReq* req = (GetSeqReq*)msg;
        GetSeqRes* res = GetSeqRes::create();
        
        std::map<int, int64_t>::iterator it;
        it = m_seq_map.find(req->seq_type);
        if (it != m_seq_map.end())
        {
            res->start = it->second;
            res->count = req->count;
            it->second += req->count;
        }
        else
        {
            m_seq_map[req->seq_type] = 0;
            res->start = 0;
            res->count = req->count;
            m_seq_map[req->seq_type] += res->count;
        }
        
        LOG_DEBUG("Received GetSeqReq, seq_type:" << req->seq_type
            << ", count:" << req->count
            << ", sending GetSeqRes, start:" << res->start
            << ", count:" << res->count);
        int r = m_rpc->send_res(rpc_head, 0, res);
        if (r != 0)
        {
            LOG_ERROR("Failed to send req");
            //MsgPool<GetSeqReq>::free_msg(req);
            req->free();
        }
        LOG_DEBUG("msg has been destroyed successfully");
        //MsgPool<GetSeqReq>::free_msg(msg);
        req->free();
        
        ++send_times;
        if (send_times == 10000)
        {
            sleep(10);
            //m_rpc->output_stat();
        }
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
    LOG_DEBUG("Received msg, type:" << msg->get_msg_type()
        << ", res_code:" << res_code 
        << ", trans_param:" << trans_param
        << ", transaction_id:" << transaction_id);
    
    return 0;
}            

int main()
{
    zonda::logger::LoggerFactory::init("seq_service", 
        "seq_service.conf", false);
        
    ServiceBase<BizHandler> s("seq_service");
    s.reg_service(zonda::service_type::SEQ_SERVICE);
    s.run();
}
