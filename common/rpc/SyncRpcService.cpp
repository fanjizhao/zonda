
#include "log/Logger.h"
#include "SyncRpcService.h"


namespace zonda
{
namespace common
{


SyncAsyncConvertItem::SyncAsyncConvertItem():
    cond(lock), res(NULL), res_code(0)
{
}
 
SyncRpcHandler::SyncRpcHandler()
{
}

int SyncRpcHandler::init(zonda::common::IStage* stage, void* param)
{
    return 0;
}


int SyncRpcHandler::handle_event(const zonda::common::IEvent* event)
{
    int event_type = event->get_type();
    
    if (event_type == zonda::common::EventType::RPC_EVENT)
    {
        return handle_rpc_event(event);
    }
    
    delete event;
    return 0;
}

int SyncRpcHandler::handle_rpc_req(int op_type, zonda::common::IMsg* msg, RpcHead* rpc_head)
{
    return 0;
}  
      

int SyncRpcHandler::handle_rpc_res(
            int op_type,
            int res_code,
            zonda::common::IMsg *msg,
            int64_t trans_param,
            int64_t transaction_id,
            IMsg* orignal_req)
{
    LOG_DEBUG("Received res, op_type:" << op_type
        << ", res_code:" << res_code 
        << ", trans_param:" << trans_param
        << ", transaction_id:" << transaction_id);

    SyncAsyncConvertItem* item = (SyncAsyncConvertItem*)trans_param;
    item->res = msg;
    item->res_code = res_code;
    item->cond.signal();
    return 0;
}

SyncRpcService::SyncRpcService(IRpcService* async_rpc)
{
    m_rpc = async_rpc;
    m_stage = new Stage<SyncRpcHandler>("SyncRpcStage", 1000);
 
}

int SyncRpcService::call(IMsg* req, IMsg* &res, const ServiceAddr& responser)
{
    SyncAsyncConvertItem item;
    m_rpc->send_req(m_stage, req, 0, (int64_t)&item, responser);
    item.cond.wait();
    res = item.res;
    return item.res_code;
}

int SyncRpcService::start()
{
    int r = m_stage->init(NULL);
    if (r != 0)
    {
        LOG_ERROR("Failed to init SyncRpcStage");
        return r;
    }
    
    r = m_stage->start();
    if (r != 0)
    {
        LOG_ERROR("Failed to start SyncRpcStage");
    }
    
    return r;    
}

void SyncRpcService::stop()
{
    m_stage->stop();
}
    
}//end namespace common
}//end namespace zonda

