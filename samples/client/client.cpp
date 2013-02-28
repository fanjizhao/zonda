
#include <fstream>
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
#include "log/Logger.h"
#include "config/Config.h"
#include "rpc/GlobalServicesInfo.h"
#include "rpc/SyncRpcService.h"
#include "thread/Thread.h"
#include "AsyncBizHandler.h"

using namespace zonda;
using namespace zonda::common;
using namespace zonda::msg::echo_service;
using namespace zonda::msg::seq_service;
using namespace std;

RpcService* rpc = NULL;
IStage* stage = NULL;
const int GET_SEQ_REQ_TEST_TIMES = 10000;
const int ECHO_REQ_TEST_TIMES = 100;
const int concurrent_num = 4; 
int test_times;
int m_success_times;
int m_failed_times;
struct timeval send_time[ECHO_REQ_TEST_TIMES];
struct timeval recv_time[ECHO_REQ_TEST_TIMES]; 


  
struct BizHandlerParam
{
    zonda::common::RpcService* rpc;

    int echo_fsm_count;    
};

void write_the_time_info()
{
    char buff[1204];
    sprintf(buff, "/tmp/jizhao/client_time_diff.txt");
    ofstream outfile (buff);
    struct timeval diff;

    for (int i=0; i<ECHO_REQ_TEST_TIMES; ++i)
    {
        //skip failed test 
        if (recv_time[i].tv_sec == 0) continue;
        
        diff.tv_sec = recv_time[i].tv_sec - send_time[i].tv_sec;
        diff.tv_usec = recv_time[i].tv_usec - send_time[i].tv_usec;
        if (diff.tv_usec < 0)
        {
            diff.tv_sec -= 1;
            diff.tv_usec += 1000000;
        }
        
        sprintf(buff, "%d.%06d\t%d.%06d\t%d.%06d\n",
            send_time[i].tv_sec, send_time[i].tv_usec,
            recv_time[i].tv_sec, recv_time[i].tv_usec,
            diff.tv_sec, diff.tv_usec);
            
        outfile << buff;
    }
    outfile.close();
    exit(0);
}

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
            IMsg* orignal_req);
    
private:
    
    zonda::common::RpcService* m_rpc;
    zonda::common::IStage* m_self_stage;
    string m_test_str;
   
};


BizHandler::BizHandler():
    m_rpc(NULL)
{
    m_success_times = 0;
    m_failed_times = 0;
    m_test_str.resize(1024, 'F');
    test_times = 0;
}

int BizHandler::init(zonda::common::IStage* stage, void* param)
{
    m_self_stage = stage;
    BizHandlerParam* p = (BizHandlerParam*)param;
    m_rpc = p->rpc;
    

   
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
        LOG_DEBUG("Received a timer event, param:" << e->get_param());
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
    
    return 0;
}  
      

int BizHandler::handle_rpc_res(
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
            m_success_times++;
            //cout << "m_success_times:" << m_success_times << endl;
            if (orignal_req != NULL)
            {
                EchoReq* req = (EchoReq*)orignal_req;
                LOG_DEBUG("orignal_req:" << req->str);
                req->free();
            }
        }
        else
        {
            LOG_DEBUG("Failed to recv EchoRes, transaction_id:"
                << transaction_id << ", trans_param:" << trans_param);
            cout << ", res_code:" << res_code << endl;
            m_failed_times++;
          
        }
        
        if (m_success_times + m_failed_times == ECHO_REQ_TEST_TIMES)
        {
            LOG_FATAL("Test EchoReq End. Success:" << m_success_times
                << ", Failed:" << m_failed_times);
            cout <<"Test EchoReq End. Success:" << m_success_times
                << ", Failed:" << m_failed_times << endl;                    
            write_the_time_info();
            exit(0);
            //m_rpc->output_stat();
            //Reset it
            test_times = 0;
            m_success_times = 0;
            m_failed_times = 0;            
        }    
    }
    else if (op_type == op_type::seq_service::GET_SEQ)
    {
        if (res_code == 0)
        {
            GetSeqRes* res = (GetSeqRes*)msg;
            LOG_INFO("Received GetSeqRes, start:" << res->start
                << ", count:" << res->count
                << ", trans_param:" << trans_param
                << ", transaction_id:" << transaction_id);
            res->free();
            
            if (transaction_id == GET_SEQ_REQ_TEST_TIMES)
            {
                LOG_FATAL("Test GetSeqReq End");
                //Reset it
                test_times = 0;
            }
        }
        else
        {
            LOG_DEBUG("Received error GetSeqRes, trans_param:" << trans_param
                << ", transaction_id:" << transaction_id);
        }

    }
            
    return 0;
}
void launch_echo_test()
{
    string s(1024, 'F');
    LOG_FATAL("Begin Test EchoReq");
    struct timeval now;
    for (int i=0; i<ECHO_REQ_TEST_TIMES/concurrent_num; ++i)
    {
        for (int j=0; j<concurrent_num; ++j)
        {
            test_times = i*concurrent_num + j;
            
            EchoReq* req = EchoReq::create();
            req->str = s;
            ServiceAddr responser;
            responser.service_type = zonda::service_type::ECHO_SERVICE;
            
            gettimeofday(&now, NULL);
            send_time[test_times] = now;
            //cout << "test_times:" << test_times << endl;        
            LOG_DEBUG("Send EchoReq, trans_param:" << test_times 
                << ", transaction_id:" << test_times);
    
            int r = rpc->send_req(stage, 
                req, test_times, test_times, responser, true);
            if (r != 0)
            {
                LOG_ERROR("Failed to send req" << ", transaction_id:" << test_times);
                req->free();
            }
        }
        usleep(200);
    }    
}


/*     
void launch_seq_test()
{
    LOG_FATAL("Test Begin");
    for (int i=0; i<GET_SEQ_REQ_TEST_TIMES; ++i)
    {
        GetSeqReq* seq_req = GetSeqReq::create();
        seq_req->seq_type = 0;
        seq_req->count = 1;
        ServiceAddr seq_serv_addr;
        seq_serv_addr.service_type = service_type::SEQ_SERVICE;
        seq_serv_addr.group_id = 0;
        seq_serv_addr.role = 1;
        
        
        test_times++;
        LOG_INFO("Send GetSeqReq, trans_param:" << test_times 
            << ", transaction_id:" << test_times);
        int r = rpc->send_req(stage, 
            seq_req, test_times, test_times, seq_serv_addr, 10);
        if (r != 0)
        {
            LOG_ERROR("Failed to send req" << ", transaction_id:" << test_times);
            seq_req->free();
        }
    }    
}
*/


int test_sync_response_time()
{
    int r = 0;
    zonda::MsgFactory msg_factory;
    
    zonda::common::RpcService my_rpc;
    rpc = &my_rpc;
    zonda::common::RpcServiceParam rpc_param(&msg_factory);
    
    r = my_rpc.init(&rpc_param);
    if (r != 0)
    {
        LOG_FATAL("Failed to init my_rpc");
        return 0;        
    }
    
    
    zonda::common::Stage<BizHandler> biz_stage("test_client_biz_stage", 100000);
    stage = &biz_stage;
    BizHandlerParam param;
    param.rpc = &my_rpc;
    
    
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
    
    /* 
    r = my_rpc.set_stage_service(biz_stage.get_sink(), zonda::service_type::CLIENT);
    if (r != 0)
    {
        LOG_FATAL("Failed to call set_stage_service for CLIENT");
        return -1;
    } */      

    r = my_rpc.start();
    if (r != 0)
    {
        LOG_FATAL("Failed to start my_rpc");
        return 0;            
    }   
    
    SyncRpcService sync_rpc(&my_rpc);
    r = sync_rpc.start();
    if (r != 0)
    {
        LOG_FATAL("Failed to start sync_rpc");
        return 0;          
    }
    sleep(10);
    //Test GetSeqReq
    //my_timer.schedule(biz_stage.get_sink(), 1, 10*1000);
    
    //Test EchoReq
    //my_timer.schedule(biz_stage.get_sink(), 2, 10*1000);
    //my_timer.schedule(biz_stage.get_sink(), 2, 20*1000);
    //my_timer.schedule(biz_stage.get_sink(), 2, 30*1000);
    //launch_echo_test();
    
    
    char buff[100];
    EchoRes* res = NULL;
    ServiceAddr responser;
    responser.service_type = zonda::service_type::ECHO_SERVICE;
    struct timeval now;
    string s(1024, 'F'); 
    for (int i=0; i<ECHO_REQ_TEST_TIMES; ++i)
    {
        EchoReq* req = EchoReq::create();
        //sprintf(buff, "%d", i);
        req->str = s;
        gettimeofday(&now, NULL);
        test_times = i;
        send_time[test_times] = now;
        int r = sync_rpc.call((IMsg*)req, (IMsg*&)res, responser);
        if (r != 0)
        {
            cout << "Failed to call. r=" << r << endl;
            ++m_failed_times;
            recv_time[test_times].tv_sec = 0;
        }
        else
        {
            ++m_success_times;
            //cout << "res->str=" << res->str << endl;
            delete res;
            res = NULL;
            gettimeofday(&now, NULL);
            recv_time[test_times] = now;            
        }
    }
    cout << "Test EchoReq End. Success:" << m_success_times 
    << ", Failed:" << m_failed_times << endl;
    write_the_time_info();
    exit(0);    
}
int test_async_perf()
{
    int r = 0;
    zonda::MsgFactory msg_factory;
    
    zonda::common::RpcService my_rpc;
    rpc = &my_rpc;
    zonda::common::RpcServiceParam rpc_param(&msg_factory);
    
    r = my_rpc.init(&rpc_param);
    if (r != 0)
    {
        LOG_FATAL("Failed to init my_rpc");
        return 0;        
    }
    
    
    zonda::common::Stage<AsyncBizHandler> biz_stage("test_async_perf", 100000, 2);
    stage = &biz_stage;
    BizHandlerParam param;
    param.rpc = &my_rpc;
    
    
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
    
   

    r = my_rpc.start();
    if (r != 0)
    {
        LOG_FATAL("Failed to start my_rpc");
        return 0;            
    }   
    

    sleep(10);

    
    
    char buff[100];
    EchoRes* res = NULL;
    ServiceAddr responser;
    responser.service_type = zonda::service_type::ECHO_SERVICE;
    struct timeval now;
    string s(1024, 'F'); 
    for (int64_t i=0; i<ECHO_REQ_TEST_TIMES; ++i)
    {
        EchoReq* req = EchoReq::create();
        req->str = s;
        gettimeofday(&now, NULL);
        test_times = i;
        send_time[i] = now;
        r = rpc->send_req(&biz_stage, 
            req, i, i, responser, 10);
        if (r != 0)
        {
            LOG_ERROR("Failed to send req" << ", transaction_id:" << test_times);
            req->free();
        }
    }
    
    zonda::common::ThreadMgr::instance()->wait_all();
    
}
class CallTime
{
public:
    double send_time;
    double recv_time;
};


class ConcurrentTestTask: public zonda::common::Thread
{
public:
    static const int TEST_TIMES = 100000;
    static const int THREAD_NUM = 50;
    static vector<CallTime> call_time_vector;
    static ThreadLock lock;
    static int alive_thread_num;
private:
    CallTime call_time_array[TEST_TIMES];
    string test_string;
    ServiceAddr responser;
    
public:
    SyncRpcService* sync_rpc;
    ConcurrentTestTask()
    {
        test_string.assign(1024, 'W');       
        responser.service_type = service_type::ECHO_SERVICE;
    }
    
    void stop()
    {
    }
    void run()
    {
        struct timeval now;
        for (int i=0; i<TEST_TIMES; ++i)
        {
            EchoReq* req = EchoReq::create();
            req->str = test_string;
            gettimeofday(&now, NULL);  
            call_time_array[i].send_time = now.tv_sec + (now.tv_usec / 1000000.0);
            IMsg* res;
            
            int r = sync_rpc->call((IMsg*)req, (IMsg*&)res, responser);
            if (r != 0)
            {
                cout << "Failed to call. r=" << r << endl;
                call_time_array[i].recv_time = 0;
            }
            else
            {
                //cout << "res->str=" << res->str << endl;
                res->free();
                res = NULL;
                gettimeofday(&now, NULL);  
                call_time_array[i].recv_time = now.tv_sec + (now.tv_usec / 1000000.0);
            }      
        }
        
        lock.lock();
        for (int i=0; i<TEST_TIMES; ++i)
        {
            call_time_vector.push_back(call_time_array[i]);        
        }
        lock.unlock();
               
        if (__sync_sub_and_fetch(&alive_thread_num, 1) == 0)
        {
            writeConcurrentTimeInfo();
        }
    }
    
    static void writeConcurrentTimeInfo()
    {
        char buff[1204];
        sprintf(buff, "/tmp/jizhao/client_time_diff.txt");
        ofstream outfile (buff);
        struct timeval diff;
    
        for (int i=0; i<call_time_vector.size(); ++i)
        {
            //skip failed test 
            if (call_time_vector[i].recv_time == 0) continue;
            sprintf(buff, "%f\t%f\t%f\n",
                call_time_vector[i].send_time,
                call_time_vector[i].recv_time,
                call_time_vector[i].recv_time - call_time_vector[i].send_time);
                
            outfile << buff;
        }
        outfile.close();
        exit(0);
    }
    
   
};

int ConcurrentTestTask::alive_thread_num = THREAD_NUM; 
vector<CallTime> ConcurrentTestTask::call_time_vector = vector<CallTime>();
ThreadLock ConcurrentTestTask::lock = ThreadLock();


int testConcurrentPerf()
{
    int r = 0;
    zonda::MsgFactory msg_factory;
    
    zonda::common::RpcService my_rpc;
    rpc = &my_rpc;
    zonda::common::RpcServiceParam rpc_param(&msg_factory);
    
    r = my_rpc.init(&rpc_param);
    if (r != 0)
    {
        LOG_FATAL("Failed to init my_rpc");
        return 0;        
    }
    
    
    zonda::common::Stage<BizHandler> biz_stage("test_client_biz_stage", 100000);
    stage = &biz_stage;
    BizHandlerParam param;
    param.rpc = &my_rpc;
    
    
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
    
    r = my_rpc.start();
    if (r != 0)
    {
        LOG_FATAL("Failed to start my_rpc");
        return 0;            
    }   
    
    SyncRpcService sync_rpc(&my_rpc);
    r = sync_rpc.start();
    if (r != 0)
    {
        LOG_FATAL("Failed to start sync_rpc");
        return 0;          
    }
    sleep(10);
    
    ConcurrentTestTask* test_thread_list = new ConcurrentTestTask[ConcurrentTestTask::THREAD_NUM];        
    for(int i=0; i<ConcurrentTestTask::THREAD_NUM; ++i)
    {
        test_thread_list[i].sync_rpc = &sync_rpc;
        test_thread_list[i].start();   
    }
    
    zonda::common::ThreadMgr::instance()->wait_all();
}
int main()
{
    int r = 0;
    zonda::logger::LoggerFactory::init("client", "client.conf", false);

   
    LOG_DEBUG("I test_client");
    r = Config::instance()->init(NULL);
    if (r != 0)
    {
        LOG_FATAL("Failed to init config");
        return 0;
    }
        
    
    //test_sync_response_time();
    //testConcurrentPerf();
    test_async_perf();
}
