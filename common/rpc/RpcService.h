#ifndef ZONDA_COMMON_RPC_SERVICE_H_
#define ZONDA_COMMON_RPC_SERVICE_H_


#include <arpa/inet.h>
#include <iostream>
#include <tr1/unordered_map>
#include "IRpcService.h"
#include "seda/IStage.h"
#include "seda/Stage.h"
#include "TransInEvent.h"
#include "util/byte_order.h"
#include "util/TimerService.h"
#include "util/TimerEvent.h"
#include "log/Logger.h"
#include "config/ConfigChangeEvent.h"
#include "TransHead.h"
#include "RpcHead.h"
#include "ZMQSender.h"
#include "ZMQReceiver.h"
#include "CallOutRecord.h"


#define MEM_BLOCK_SIZE 10485760 //10MB  
    
namespace zonda
{
namespace common
{

using namespace std;
using namespace std::tr1;     


//class CallOutRecord;


class SendRpcReqEvent: public IEvent
{
public:
    SendRpcReqEvent();
    virtual int get_type() const;
    ServiceAddr invoker;
    ServiceAddr responser;
    IMsg*       msg;
    int16_t     biz_handler_id;
    int64_t     transaction_id;
    int64_t     trans_param;
    int         timeout;
    int64_t     call_id;
    bool        return_req;
};

class SendRpcResEvent: public IEvent
{
public:
    SendRpcResEvent();
    virtual int get_type() const;
    RpcHead* req_rpc_head;
    int res_code;
    IMsg* msg;
};

struct RpcStageHandlerParam
{
    RpcServiceParam* rpc_param;
    CallOutRecord*   call_out_record;
};

class RpcStageHander: public IEventHandler
{
public:
    RpcStageHander();
    ~RpcStageHander();
    virtual int init(IStage* stage, void* param);
    virtual int handle_event(const IEvent* event);

private:
    int handle_send_rpc_req_event(const SendRpcReqEvent* e);
    int handle_send_rpc_res_event(const SendRpcResEvent* e);
    int handle_timer_event(const TimerEvent* e);
    int handle_conf_change(const ConfigChangeEvent* e);   
    
    //Create an error response and throw it back
    void create_err_res(const SendRpcReqEvent* e, int16_t res_code, int req_msg_type);
private:
    ITransService* m_trans;
    IMsgFactory* m_msg_factory;
    CallOutRecord*   m_call_out_record;
    uint32_t    m_call_in_count;
    ITimerService* m_timer;
    IEventSink* m_self_sink;
    int16_t m_stage_id;
    ZMQSender   m_trans_sender;
    uint8_t*     m_buff;
    
    int64_t m_client_instance_id;
    
};



class RpcService: public IRpcService
{
public:
    RpcService();
    ~RpcService();
    
    virtual int init(RpcServiceParam* param);
    
    virtual int start();
    virtual void stop();
    //!service_type will be binded to all listen tcp_addr and ipc_addr.
    virtual int set_stage_service(IStage* request_stage,
        int service_type);
    
    //!service_type will be binded to ip:port and ipc_addr.
    virtual int set_stage_service(IStage* request_stage,
        int service_type, 
        const char* ip, 
        unsigned short port,
        const char* ipc_addr);
        
    virtual int set_stage_service(IStage* request_stage,
        int service_type,
        std::vector<std::pair<std::string, uint16_t> >& tcp_addr_list,
        std::vector<std::string>& ipc_addr_list);        
                
    virtual int send_req(IStage* response_stage, IMsg* req, 
        int64_t transaction_id, int64_t trans_parm,
        const ServiceAddr& invoker, const ServiceAddr& responser,
        bool return_req = false, int timeout = 10);

    virtual int send_req(IStage* response_stage, IMsg* req, 
        int64_t transaction_id, int64_t trans_parm,
        const ServiceAddr& responser,
        int timeout = 10);
                
    virtual int send_req(IStage* response_stage, IMsg* req,
        int64_t transaction_id, int64_t trans_parm,
        const ServiceAddr& responser, bool return_req,
        int timeout = 10);
            
    virtual int send_req(IStage* response_stage, IMsg* req, 
        int64_t trans_parm,
        const ServiceAddr& invoker, const ServiceAddr& responser,
        int timeout = 10);
    
    virtual int send_req(IStage* response_stage, IMsg* req, 
        int64_t trans_parm,
        const ServiceAddr& responser,
        int timeout = 10);
                
    virtual int send_res(RpcHead* rpc_head, int res_code, IMsg* res);     

    void output_stat() {m_trans_receiver.output_stat();}
private:
    vector<IStage*> m_rpc_stage_list;
    IEventSink*     m_default_rpc_sink;
    ZMQReceiver m_trans_receiver;
    TimerService m_timer;
    int64_t m_call_id;
    size_t m_max_call_out;     
    size_t m_max_call_in;
    uint16_t m_handler_count;
};







}//end namespace common
}//end namespace zonda

#endif //ZONDA_COMMON_IRPC_SERVICE_H_

