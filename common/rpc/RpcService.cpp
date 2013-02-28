
#include <zmq.h>
#include "RpcService.h"
#include "seda/EventType.h"
#include "ServiceStageMap.h"
#include "RouteTable.h"
#include "RpcEvent.h"
#include "config/Config.h"
#include "RouteStrategy.h"
#include "ServiceGroupingTable.h"
#include "util/GlobalTimer.h"
//#include "CallOutRecord.h"


#define TIMEOUT_TIMER 1
#define TIMEOUT_CHECK_INTERVAL 10

using namespace zonda::common;


SendRpcReqEvent::SendRpcReqEvent():
    msg(NULL),
    biz_handler_id(-1),
    transaction_id(0),
    trans_param(0),
    timeout(10),
    return_req(false)
{
}


int SendRpcReqEvent::get_type() const
{
    return EventType::SEND_RPC_REQ_EVENT;
}

SendRpcResEvent::SendRpcResEvent():
    req_rpc_head(0),
    res_code(0),
    msg(0)
{
}

int SendRpcResEvent::get_type() const
{
    return EventType::SEND_RPC_RES_EVENT;
}



RpcStageHander::RpcStageHander()
{
    m_client_instance_id = 0;
    m_call_in_count = 0;
    m_buff = new uint8_t[MEM_BLOCK_SIZE+sizeof(TransHead)+sizeof(RpcHead)];
}

RpcStageHander::~RpcStageHander()
{
    delete [] m_buff;
}

int RpcStageHander::init(IStage* stage, void* param)
{
    RpcStageHandlerParam* stage_param = (RpcStageHandlerParam*)param;
    
    RpcServiceParam* p = (RpcServiceParam*)(stage_param->rpc_param);
    m_msg_factory = p->msg_factory;
    m_timer = GlobalTimer::instance();
    m_call_out_record = stage_param->call_out_record;
    
    m_self_sink = stage->get_sink();
    m_stage_id = stage->get_id();
    
    int r = m_trans_sender.init(NULL);
    

    r = m_timer->schedule(m_self_sink, TIMEOUT_TIMER, 1*1000, TIMEOUT_CHECK_INTERVAL*1000);
    if ( r < 0)
    {
        LOG_FATAL("Failed to set timer for time out check");
    }
    else r = 0;
    
    return r;
}

int RpcStageHander::handle_event(const IEvent* event)
{
    if (event == NULL)
    {
        LOG_ERROR("event is NULL");
        return -1;
    }
    
    int r = 0;
    const TimerEvent* e= NULL;


    switch (event->get_type())
    {
    case EventType::SEND_RPC_REQ_EVENT:
        r = handle_send_rpc_req_event(dynamic_cast<const SendRpcReqEvent*>(event));
        break;
    case EventType::SEND_RPC_RES_EVENT:
        r = handle_send_rpc_res_event(dynamic_cast<const SendRpcResEvent*>(event));
        break;
    case EventType::TIMER_EVENT:
        e = dynamic_cast<const TimerEvent*>(event);
        r = handle_timer_event(e);
        break;
    case EventType::CONFIG_CHANGE_EVENT:
        r = handle_conf_change(dynamic_cast<const ConfigChangeEvent*>(event));
        break;
    default:
        LOG_ERROR("Unknow event, type: " << event->get_type());
        break;
    }
    
    delete event;
    
    return 0;
}

int RpcStageHander::handle_conf_change(const ConfigChangeEvent* e)
{
    LOG_INFO("Received conf change event, change_type:" << e->m_change_type);
    if (e->m_change_type != ConfigChangeEvent::CHANGED )
    {
        LOG_ERROR("Should not received such type conf change event, ignore!");
        return 0;
    }
    
    int r = 0;
    
    if (e->m_path.compare(Config::ZONDA_ROUTE_TABLE_PATH) == 0)
    {
        LOG_INFO("route_table changed, new size:" << e->m_value_len);
        r = RouteTable::instance()->load(e->m_value, e->m_value_len);
        if (r != 0)
        {
            LOG_ERROR("Failed to update RouteTable");
        }
        else
        {
            LOG_INFO("RouteTable updated successfully!");
        }
    }
    else if (e->m_path.compare(Config::ZONDA_ROUTE_STRATEGY_PATH) == 0)
    {
        LOG_INFO("route strategy changed, new size:" << e->m_value_len);
        r = RouteStrategy::instance()->load(e->m_value, e->m_value_len);
        if (r != 0)
        {
            LOG_FATAL("Failed to load RouteStrategy. Data=\n" << e->m_value);
            return -1;
        }
    }
    else
    {
        LOG_WARN("Did not watch the data of " << e->m_path << ", so ignore!");
    }  
    return 0;
    
}
int RpcStageHander::handle_timer_event(const TimerEvent* e)
{
    //LOG_ERROR("timer_param:" << e->get_param()
    //    << ", call_out_record.size=" << m_call_out_record->size() );
    size_t s = m_call_out_record->size();
    struct timeval t1, t2, t3;
    gettimeofday(&t1, NULL);
    m_call_out_record->handle_timeout_items();
    gettimeofday(&t2, NULL);
    t3.tv_sec = t2.tv_sec - t1.tv_sec;
    t3.tv_usec = t2.tv_usec - t1.tv_usec;
    if (t3.tv_usec < 0)
    {
        t3.tv_sec -=1;
        t3.tv_usec += 1000000;
    }
    //LOG_ERROR("handle_timeout_items, size:" 
    //    << s << ", time: " << t3.tv_sec << "." << t3.tv_usec);
    return 0;
}

void RpcStageHander::create_err_res(const SendRpcReqEvent* e, int16_t res_code, int req_msg_type)
{
    //Note: Here we need the msg instead of getting msg_type from
    //since the msg is NULL when e->return_req is false
    RpcHead* rpc_head = new RpcHead();
    rpc_head->msg_type = 0 - req_msg_type;
    rpc_head->trans_param = e->trans_param;
    rpc_head->transaction_id = e->transaction_id;
    rpc_head->res_code = res_code;
    rpc_head->biz_handler_id = e->biz_handler_id;
    
    RpcEvent* re = new RpcEvent(rpc_head);
    re->set_req(e->msg);
    IEventSink* sink = NULL;
    sink = StageMgr::instance()->get_stage(rpc_head->biz_handler_id)->get_sink();
    
    sink->enqueue(re);      
}

int RpcStageHander::handle_send_rpc_req_event(const SendRpcReqEvent* e)
{
    LOG_DEBUG("msg_type:" << e->msg->get_msg_type()
        << ", transaction_id:" << e->transaction_id
        << ", trans_param:" << e->trans_param
        << ", call_id:" << e->call_id);

    std::string host;
    uint16_t port;
    std::string ipc_addr;
    
    int r = 0;
    r = RouteTable::instance()->get_real_addr(e->responser, host, port);
    if (r != 0)
    {
        LOG_ERROR("Failed to get_real_addr, service_type: " << e->responser.service_type);
        e->msg->free();
        return -1;
    }
    
    LOG_DEBUG("Begin to make the packet");
    int data_len = 0;
    TransHead trans_head;
    trans_head.version = 1;
    trans_head.rpc_head_len = sizeof(RpcHead);
  	trans_head.call_id = e->call_id;
    
    //We must recored the msg_type since the msg factory
    //will free the msg after the serialization
    int msg_type = e->msg->get_msg_type();
    r = m_msg_factory->serialize_body(
        m_buff+sizeof(TransHead)+sizeof(RpcHead), 
        MEM_BLOCK_SIZE, e->msg, data_len, e->return_req);
    if (r != 0)
    {
        LOG_ERROR("Failed to call msg factory serialize_body");
        //TODO: Produce the fake res and throw it back
        create_err_res(e, RpcResCode::MSG_TOO_BIG, msg_type);
        return -1;
    }
    
    trans_head.rpc_msg_len = data_len;
    LOG_DEBUG("Begin to send a msg, TransHead, version:" << trans_head.version
        << ", rpc_head_len:" << trans_head.rpc_head_len
        << ", rpc_msg_len:" << trans_head.rpc_msg_len
        << ", msg size:" << (trans_head.rpc_head_len+trans_head.rpc_msg_len+sizeof(TransHead)));
            
    RpcHead* rpc_head = new RpcHead();
	rpc_head->msg_type = msg_type;
	rpc_head->responser = e->responser;
	rpc_head->invoker = e->invoker;
		
	if (rpc_head->invoker.service_type == 0)
	{
	    if (m_client_instance_id == 0)
	    {
    	        r= ServiceStageMap::instance()->get_self_instance_id(0, m_client_instance_id);
    	        if (r != 0)
    	        {
    	            delete rpc_head;
    	            if (e->return_req)
    	            {
    	                e->msg->free();
    	            }
    	            LOG_ERROR("Failed to get my client service instance id");
    	            return -1;
    	        }
    	    }
	        rpc_head->invoker.instance_id = m_client_instance_id;
        }

	rpc_head->transaction_id = e->transaction_id;
	rpc_head->trans_param = e->trans_param;
	rpc_head->call_id = e->call_id;
	rpc_head->biz_handler_id = e->biz_handler_id;

    
    trans_head.hton_serialize(m_buff);
    rpc_head->hton_serialize(m_buff+sizeof(TransHead));
       

    LOG_DEBUG("Begin to send a msg, RpcHead, msg_type:" << rpc_head->msg_type
        << ", call_id:" << rpc_head->call_id
        << ", transaction_id:" << rpc_head->transaction_id
        << ", responser_type:" << rpc_head->responser.service_type
        << ", invoker_type:" << rpc_head->invoker.service_type
        << ", biz_handler_id:" << rpc_head->biz_handler_id);

    CallOutRecordItem out_record_item;
    out_record_item.rpc_head = rpc_head;
    out_record_item.send_time = time(NULL);
    out_record_item.timeout = e->timeout;
    if (e->return_req) out_record_item.req = e->msg;
    m_call_out_record->add(out_record_item);
    
      
    int whole_data_len  = sizeof(TransHead) + trans_head.rpc_head_len + trans_head.rpc_msg_len;

   
    
    r = m_trans_sender.send_data(rpc_head->call_id, m_buff,  
        whole_data_len, 
        host.c_str(), port);
        
    if (r != 0)
    {
        m_call_out_record->remove(rpc_head->call_id, false);
        LOG_ERROR("Failed to send_data to trans service");
        rpc_head->msg_type = 0 - rpc_head->msg_type;
        rpc_head->res_code = RpcResCode::SEND_FAIL;
        
        RpcEvent* rpc_event = new RpcEvent(rpc_head);
        if(e->return_req)
        {
            rpc_event->set_req(e->msg);
        }
        IEventSink* sink = NULL;
        sink = StageMgr::instance()->get_stage(rpc_head->biz_handler_id)->get_sink();
        
        sink->enqueue(rpc_event);         

        //This rec_head has been used to as the rpc_head of res
        //so don't need to delete
    }
    return r;
}

int RpcStageHander::handle_send_rpc_res_event(const SendRpcResEvent* e)
{
    LOG_DEBUG("call_id:" << e->req_rpc_head->call_id
    << ", res_code:" << e->res_code
    << ", msg_type:" << (e->msg ? e->msg->get_msg_type() : 0));

    
    RpcHead* req_rpc_head = e->req_rpc_head;
    
    std::string host;
    uint16_t port;
    std::string ipc_addr;
    ServiceAddr req_invoker_addr;
    req_invoker_addr = req_rpc_head->invoker;
        
    int r = 0;
    r = RouteTable::instance()->get_real_addr(req_invoker_addr, host, port);
    if (r != 0)
    {
        LOG_ERROR("Failed to get_real_addr, service_type: " << req_invoker_addr.service_type);
        --m_call_in_count;
        delete req_rpc_head;
        e->msg->free();
        return -1;
    }
    
    //1MB
    int data_len = 0;
    TransHead trans_head;
    trans_head.version = 1;
    trans_head.rpc_head_len = sizeof(RpcHead);
    trans_head.call_id = e->req_rpc_head->call_id;
    
    r = m_msg_factory->serialize_body(
        m_buff+sizeof(TransHead)+sizeof(RpcHead), 
        MEM_BLOCK_SIZE, e->msg, data_len, false);
    if (r != 0)
    {
        LOG_ERROR("Failed to call msg factory serialize_body");
        --m_call_in_count;
        delete req_rpc_head;
        return -1;
    }
    
    trans_head.rpc_msg_len = data_len;
    LOG_DEBUG("Begin to send an res msg, TransHead, version:" << trans_head.version
        << ", rpc_head_len:" << trans_head.rpc_head_len
        << ", rpc_msg_len:" << trans_head.rpc_msg_len
        << ", msg size:" << (trans_head.rpc_head_len+trans_head.rpc_msg_len+sizeof(TransHead)));
    
    
    //NOTE:Reuse the req rpc head as the res rpc head
    RpcHead* rpc_head = req_rpc_head;
	rpc_head->msg_type = 0 - rpc_head->msg_type;
	ServiceAddr sa = rpc_head->responser;
	rpc_head->responser = rpc_head->invoker;
	rpc_head->invoker= sa;
	rpc_head->res_code = 0;
    
        
    
    trans_head.hton_serialize(m_buff);
    rpc_head->hton_serialize(m_buff+sizeof(TransHead));
    
    LOG_DEBUG("Begin to send an res msg, RpcHead, msg_type:" << rpc_head->msg_type
        << ", call_id:" << rpc_head->call_id
        << ", transaction_id:" << rpc_head->transaction_id
        << ", trans_param:" << rpc_head->trans_param);
        
    int whole_data_len  = sizeof(TransHead) + trans_head.rpc_head_len + trans_head.rpc_msg_len;
    r = m_trans_sender.send_data(rpc_head->call_id, m_buff,  
        whole_data_len, 
        host.c_str(), port);
        
    if (r != 0)
    {
        LOG_ERROR("Failed to send_data to trans service");
    }

    --m_call_in_count;
    delete req_rpc_head;

    return r;
    
}


 
//RpcService
/////////////////////////////////////////////
RpcService::RpcService()    
{


}

RpcService::~RpcService()
{
      
}

int RpcService::init(RpcServiceParam* param)
{
    GlobalTimer::instance()->init();
    //TODO::Use a Singleton to manage the zmq context
    ZMQSender::m_zmq_context = zmq_init(1);
    if(ZMQSender::m_zmq_context == NULL)
    {
        LOG_FATAL("Failed to call zmq_init, error:" << zmq_strerror(errno));
        return -1;        
    }
    
    
    m_handler_count = param->max_handler_count;
    m_max_call_out = param->max_call_out;
    m_max_call_in = param->max_call_in;
    
    if (m_handler_count == 0)
    {
        LOG_FATAL("m_handler_count must not be zero!!");
        return -1;
    }
    
    TransReceiverParm trans_param; 
    RpcStageHandlerParam stage_param;
    stage_param.rpc_param = param;

    int r = 0;
    
    Stage<RpcStageHander>* rpc_stage = NULL;
    size_t queue_capacity = (2*m_max_call_out + m_max_call_in )/m_handler_count;
    for (uint16_t i=0; i<m_handler_count; ++i)
    {
        stage_param.call_out_record = new CallOutRecord();
        trans_param.call_out_record_list.push_back(stage_param.call_out_record);
        
        char buff[20];
        sprintf(buff, "rpc_stage_%d", i);
        rpc_stage = new Stage<RpcStageHander>(buff, queue_capacity);
        r = rpc_stage->init(&stage_param);
        if (r!= 0)
        {
            LOG_FATAL("Failed to init m_rpc_stage");
            return r;
        }
        m_rpc_stage_list.push_back(rpc_stage);
    }
    
        

    m_default_rpc_sink = m_rpc_stage_list[0]->get_sink();
   
    

    if (param->ip.size() != 0)
    {
        param->tcp_addr_list.push_back(std::make_pair(param->ip, param->port));
    }
    
    if (param->ipc_addr.size() != 0)
    {
        param->ipc_addr_list.push_back(param->ipc_addr);
    }
       


    trans_param.msg_factory =  param->msg_factory; 
    trans_param.tcp_addr_list = param->tcp_addr_list;
    trans_param.ipc_addr_list = param->ipc_addr_list;

        
    r = m_trans_receiver.init(&trans_param);
    if (r!= 0)
    {
        LOG_FATAL("Failed to init m_trans");
        return r;
    }        

    std::vector<std::pair<std::string,uint16_t> > tcp_addr_list;
    std::vector<std::string> ipc_addr_list;       
    m_trans_receiver.get_listen_addr(tcp_addr_list, ipc_addr_list);
    
    RouteTable::instance()->set_local_listen_addr(tcp_addr_list, ipc_addr_list);
    ServiceStageMap::instance()->set_local_listen_addr(tcp_addr_list);
    
    //Load route strategy info
    string data;
    Config* config = Config::instance();
    r = config->get_value(Config::ZONDA_ROUTE_STRATEGY_PATH, data, m_default_rpc_sink);
    if (r != 0)
    {
        LOG_FATAL("Failed to get data of " << Config::ZONDA_ROUTE_STRATEGY_PATH);
        return -1;
    }
    
    r = RouteStrategy::instance()->load(data.c_str(), data.size());
    if (r != 0)
    {
        LOG_FATAL("Failed to load RouteStrategy. Data=\n" << data);
        return -1;
    }   
    
    
    //Load service grouping info
    r = config->get_value(Config::ZONDA_SERVICE_GROUPING_PATH, data, m_default_rpc_sink);
    if (r != 0)
    {
        LOG_FATAL("Failed to get data of " << Config::ZONDA_SERVICE_GROUPING_PATH);
        return -1;
    }    
    r = ServiceGroupingTable::instance()->load(data.c_str(), data.size());
    if (r != 0)
    {
        LOG_FATAL("Failed to load ServiceGroupingTable. Data=\n" << data);
        return -1;
    }       
    
    std::string route_table_data;
    LOG_DEBUG("To set watch on " << Config::ZONDA_ROUTE_TABLE_PATH);
    r = config->get_value(Config::ZONDA_ROUTE_TABLE_PATH, route_table_data, m_default_rpc_sink);
    if ( r != 0)
    {
        LOG_FATAL("Failed to get route_table_data");
        return r;
    }
    
    r = RouteTable::instance()->load(route_table_data.c_str(), route_table_data.size());
    if ( r != 0)
    {
        LOG_FATAL("RouteTable failed to load data");
        return r;
    }
        
    struct timeval now;
    gettimeofday(&now, NULL);
    //The initial value for the call_id
    m_call_id = now.tv_sec*1000000 + now.tv_usec;     
    return 0;
}
    
int RpcService::start()
{
    int r = 0;
    
    r = m_timer.start();
    if (r != 0)
    {
        LOG_FATAL("Failed to start m_timer");
        return r;        
    }

    for (size_t i=0; i<m_rpc_stage_list.size(); ++i)
    {
        r = m_rpc_stage_list[i]->start();
        if (r!= 0)
        {
            LOG_FATAL("Failed to start m_rpc_stage");
            return r;
        }
    }
        

    r = m_trans_receiver.start();
    if (r!= 0)
    {
        LOG_FATAL("Failed to start m_trans_receiver");
    }    
    
    return r;
}

void RpcService::stop()
{
    m_timer.stop();
    for (size_t i=0; i<m_rpc_stage_list.size(); ++i)
    {
        m_rpc_stage_list[i]->stop();
    }
    m_trans_receiver.stop();
}

//!service_type will be binded to all listen tcp_addr and ipc_addr.
int RpcService::set_stage_service(IStage* request_stage,
    int service_type)
{
    std::vector<std::pair<std::string,uint16_t> > tcp_addr_list;
    std::vector<std::string> ipc_addr_list;    
    m_trans_receiver.get_listen_addr(tcp_addr_list, ipc_addr_list);    
    
    int r = 0;
    r = set_stage_service(
        request_stage, service_type,
        tcp_addr_list, ipc_addr_list);

    return r;
}

//!service_type will be binded to ip:port and ipc_addr.
int RpcService::set_stage_service(IStage* request_stage,
    int service_type, 
    const char* ip, 
    unsigned short port,
    const char* ipc_addr)
{
    std::vector<std::pair<std::string, uint16_t> > tcp_addr_list;
    std::vector<std::string> ipc_addr_list;
    
    tcp_addr_list.push_back(std::make_pair(ip, port));
    ipc_addr_list.push_back(ipc_addr);
    int r = 0;
    r = set_stage_service(
        request_stage, service_type,
        tcp_addr_list, ipc_addr_list);

    return r;  
}
    
int RpcService::set_stage_service(IStage* request_stage,
    int service_type,
    std::vector<std::pair<std::string, uint16_t> >& tcp_addr_list,
    std::vector<std::string>& ipc_addr_list)
{
    int r = 0;
    std::set<std::pair<std::string,uint16_t> > tcp_addr_set; 
    std::set<std::string> ipc_addr_set;    
    
    m_trans_receiver.get_listen_addr(tcp_addr_set, ipc_addr_set);
    
    std::set<std::pair<std::string,uint16_t> >::iterator it1;
    for (size_t i=0; i<tcp_addr_list.size(); ++i)
    {
        LOG_DEBUG("Check " << tcp_addr_list[i].first << ":" << tcp_addr_list[i].second);
        it1 = tcp_addr_set.find(tcp_addr_list[i]);
        if (it1 == tcp_addr_set.end())
        {
            LOG_FATAL("Disallow to bind service on the addr which m_trans does not listen on"
                << ", service:" << service_type
                << ", addr: " << tcp_addr_list[i].first
                << ":" << tcp_addr_list[i].second);
            return -1;
        }
    }

    std::set<std::string>::iterator it2;
    for (size_t i=0; i<ipc_addr_list.size(); ++i)
    {
        it2 = ipc_addr_set.find(ipc_addr_list[i]);
        if (it2 == ipc_addr_set.end())
        {
            LOG_FATAL("Disallow to bind service on the addr which m_trans does not listen on"
                << ", service:" << service_type
                << ", addr: " << ipc_addr_list[i]);
            return -1;
        }
    }
    

    
    
    r = ServiceStageMap::instance()->set_stage_service(
        request_stage, service_type,
        tcp_addr_list, ipc_addr_list);
    
    if (r != 0)
    {
        LOG_FATAL("Failed to call set_stage_service");
        return r;
    }
    


    return r;     
}
        
 

   
                
int RpcService::send_req(IStage* response_stage, IMsg* req, 
        int64_t transaction_id, int64_t trans_param,
        const ServiceAddr& invoker, const ServiceAddr& responser,
        bool return_req, int timeout)
{
    if (req == NULL)
    {
        LOG_WARN("NULL req, denied! transaction_id:" << transaction_id
            << ", trans_param:" << trans_param);
        return -1;
    }
    
    int64_t call_id = __sync_fetch_and_add(&m_call_id,1);
    size_t target_rpc_stage = call_id%m_rpc_stage_list.size();
    
    LOG_DEBUG("transaction_id:" << transaction_id
        << ", trans_param:" << trans_param
        << ", msg_type:" << req->get_msg_type()
        << ", call_id:" << call_id
        << ", return_req:" << return_req
        << ", target_rpc_stage:" << target_rpc_stage
        << ", biz_stage_id:" << response_stage->get_id()); 
                
    SendRpcReqEvent* e = new SendRpcReqEvent();
    e->invoker = invoker;
    e->responser = responser;
    e->msg = req;
    e->transaction_id = transaction_id;
    e->trans_param = trans_param;
    e->biz_handler_id = response_stage->get_id();
    e->timeout = timeout;
    e->call_id = call_id;
    e->return_req = return_req;
        
    m_rpc_stage_list[target_rpc_stage]->get_sink()->enqueue(e);

    return 0;    
}

int RpcService::send_req(IStage* response_stage, IMsg* req, 
        int64_t transaction_id, int64_t trans_parm,
        const ServiceAddr& responser,
        int timeout)
{
    ServiceAddr client_addr;   
    return send_req(response_stage, 
        req, 
        transaction_id,
        trans_parm,
        client_addr,
        responser,
        false,
        timeout);
}

int RpcService::send_req(IStage* response_stage, IMsg* req,
    int64_t transaction_id, int64_t trans_parm,
    const ServiceAddr& responser, bool return_req,
    int timeout)
{
    ServiceAddr client_addr;   
    return send_req(response_stage, 
        req, 
        transaction_id,
        trans_parm,
        client_addr,
        responser,
        return_req,
        timeout);    
}
                  
int RpcService::send_req(IStage* response_stage, IMsg* req, 
        int64_t trans_parm,
        const ServiceAddr& invoker, const ServiceAddr& responser,
        int timeout)
{
    
    return send_req(response_stage, 
        req, 
        0, //default transaction id is 0
        trans_parm,
        invoker,
        responser,
        timeout);    
}

    
int RpcService::send_req(IStage* response_stage, IMsg* req, 
        int64_t trans_parm,
        const ServiceAddr& responser,
        int timeout)
{
    ServiceAddr client_addr;
   
    return send_req(response_stage, 
        req, 
        0, //default transaction id is 0
        trans_parm,
        client_addr,
        responser,
        timeout);   
}
                
int RpcService::send_res(RpcHead* req_rpc_head, int res_code, IMsg* res)
{
    size_t target_rpc_stage = req_rpc_head->call_id % m_rpc_stage_list.size(); 
        
    LOG_DEBUG("call_id:" << req_rpc_head->call_id
        << ", res_code:" << res_code
        << ", msg_type:" << (res ? res->get_msg_type(): 0)
        << ", target_rpc_stage:" << target_rpc_stage);
           
    SendRpcResEvent* e = new SendRpcResEvent();
    e->req_rpc_head = req_rpc_head;
    e->res_code = res_code;
    e->msg = res;
    

    m_rpc_stage_list[target_rpc_stage]->get_sink()->enqueue(e);
        
    return 0;
}

