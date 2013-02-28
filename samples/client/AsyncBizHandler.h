#include "rpc/RpcEventHandlerBase.h"
#include "seda/Stage.h"
#include "rpc/RpcService.h"
#include "log/Logger.h"
/*
#include "rpc/ServiceStageMap.h"
#include "rpc/RouteTable.h"
#include "inc/MsgFactory.h"
#include "thread/ThreadMgr.h"
#include "util/TimerService.h"
#include "util/TimerEvent.h"
#include "inc/services.h"
#include "util/MsgPool.h"
#include "util/FsmMgr.h"
#include "log/Logger.h"
#include "config/Config.h"
#include "rpc/GlobalServicesInfo.h"
#include "rpc/SyncRpcService.h"
#include "thread/Thread.h"*/

using namespace zonda;
using namespace zonda::common;
using namespace zonda::msg::echo_service;
using namespace zonda::msg::seq_service;
using namespace std;

extern const int ECHO_REQ_TEST_TIMES;
extern struct timeval send_time[];
extern struct timeval recv_time[];

extern void write_the_time_info();
class AsyncBizHandler: public zonda::common::RpcEventHandlerBase
{
public:
    AsyncBizHandler()
    {
        m_success_times = 0;
        m_failed_times = 0;        
    };
    virtual int init(zonda::common::IStage* stage, void* param)
    {
        m_self_stage = stage;
        m_rpc = (RpcService*)param;
        return 0;
    }
    virtual int handle_event(const zonda::common::IEvent* event)
    {
        int event_type = event->get_type();
        
        if (event_type == zonda::common::EventType::RPC_EVENT)
        {
            return handle_rpc_event(event);
        }
        
        delete event;
        return 0;        
    }
    virtual ~AsyncBizHandler() {};
    
    virtual int handle_rpc_req(
        int op_type, 
        IMsg* msg, 
        RpcHead* rpc_head){}
    
    //! When an rpc response comes back, the method is called   
    virtual int handle_rpc_res(int op_type,
            int res_code,
            IMsg *msg,
            int64_t trans_param,
            int64_t transaction_id,
            IMsg* orignal_req)
    {
        LOG_DEBUG("Received res, op_type:" << op_type
            << ", res_code:" << res_code 
            << ", trans_param:" << trans_param
            << ", transaction_id:" << transaction_id);        

        if (op_type == zonda::op_type::echo_service::ECHO)
        {
            struct timeval now; 
            gettimeofday(&now, NULL);
            recv_time[transaction_id] = now;
            if (res_code == 0)
            {
                EchoRes* res = (EchoRes*)msg;
                LOG_DEBUG("Received, EchoRes, str_len:" << res->str.size()
                    << ", trans_param:" << trans_param
                    << ", transaction_id:" << transaction_id);
                //LOG_DEBUG("msg has been destroyed successfully");
                msg->free();
                __sync_fetch_and_add(&m_success_times, 1);
                //cout << "thread_id:" << Thread::self_id() << ", m_success_times:" << m_success_times << endl;
            }
            else
            {
                LOG_DEBUG("Failed to recv EchoRes, transaction_id:"
                    << transaction_id << ", trans_param:" << trans_param
                    << ", res_code:" << res_code);
                __sync_fetch_and_add(&m_failed_times, 1);
              
            }
            
            if (m_success_times + m_failed_times == ECHO_REQ_TEST_TIMES)
            {
                m_lock.lock();
                LOG_FATAL("Test EchoReq End. Success:" << m_success_times
                    << ", Failed:" << m_failed_times);
                write_the_time_info();
                m_lock.unlock();
            }    
        }
    }
    
private:
    
    RpcService* m_rpc;
    IStage* m_self_stage;
    static int m_success_times;
    static int m_failed_times;
    static ThreadLock m_lock;
};

int AsyncBizHandler::m_success_times = 0;
int AsyncBizHandler::m_failed_times = 0;
ThreadLock AsyncBizHandler::m_lock = ThreadLock();