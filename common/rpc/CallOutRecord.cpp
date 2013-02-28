
#include "CallOutRecord.h"

using namespace zonda::common;

int CallOutRecord::add(const CallOutRecordItem& item)
{
    m_lock.lock();
    //LOG_ERROR("call_id:" << item.rpc_head->call_id);
    m_map[item.rpc_head->call_id] = item;
    m_lock.unlock();
    return 0;
}

int CallOutRecord::remove(int64_t call_id, IMsg* &req)
{
    unordered_map<int64_t, CallOutRecordItem>::iterator it;
    int r = 0;
    m_lock.lock();
    it = m_map.find(call_id);
    if (it != m_map.end())
    {
        //Free the rpc_head, which is the record for thr out req
        req = it->second.req;
        delete it->second.rpc_head;
        m_map.erase(it);
    }
    else
    {
        r = -1;
    }
        
    m_lock.unlock();
    return r;
}

int CallOutRecord::remove(int64_t call_id, bool free_rpc_head)
{
    unordered_map<int64_t, CallOutRecordItem>::iterator it;
    int r = 0;
    m_lock.lock();
    it = m_map.find(call_id);
    if (it != m_map.end())
    {
        //Free the rpc_head, which is the record for thr out req
        if (free_rpc_head) delete it->second.rpc_head;
        m_map.erase(it);
    }
    else
    {
        r = -1;
    }
        
    m_lock.unlock();
    return r;    
}

int CallOutRecord::handle_timeout_items()
{
    unordered_map<int64_t, CallOutRecordItem>::iterator it;
    time_t now = time(NULL);
    m_lock.lock();
    for (it = m_map.begin(); it != m_map.end(); )
    {
        //LOG_ERROR("now:" << now << ", send_time:" << it->second.send_time
        //    << ", timeout:" << it->second.timeout);
        if (now - it->second.send_time >= 30 /*it->second.timeout*/ )
        {
            //We need to make a fake res for the real invoker stage
            //We reuse the rpc_head of the req
            RpcHead* rpc_head = it->second.rpc_head;
            rpc_head->msg_type = 0 - rpc_head->msg_type;
            rpc_head->res_code = RpcResCode::TIMEOUT;
            LOG_INFO("call_id:" << rpc_head->call_id 
                << " times out. biz_handler_id:" 
                << rpc_head->biz_handler_id);
                
            RpcEvent* rpc_event = new RpcEvent(rpc_head);
            rpc_event->set_req(it->second.req);
            m_map.erase(it++);
            
            IEventSink* sink = NULL;
            sink = StageMgr::instance()->get_stage(rpc_head->biz_handler_id)->get_sink();
            
            sink->enqueue(rpc_event);   
            
                      
        }
        else
        {
            ++it;
        }
    }
    m_lock.unlock();
    return 0;
}

size_t CallOutRecord::size()
{
    size_t s = 0;
    m_lock.lock();
    s = m_map.size();
    m_lock.unlock();
    return s;
}
