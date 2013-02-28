#ifndef ZONDA_COMMON_IRPC_SERVICE_H_
#define ZONDA_COMMON_IRPC_SERVICE_H_


#include <stdint.h>
#include <vector>
#include <string>
#include <utility>
#include "seda/IEventSink.h"
#include "seda/IEventSource.h"
#include "seda/IStage.h"
#include "ITransService.h"
#include "IMsgFactory.h"
#include "ServiceAddr.h"
#include "RpcHead.h"

namespace zonda
{
namespace common
{


class RpcServiceParam
{
public:
    //!@param ip if listen_ip is NULL, RPC will listen on 0.0.0.0
    //!@param port if listen_port is 0, RPC will automatically choose an 
    //!available port to use
    //!@param ipc_addr if NULL, RPC will not listen on any ipc addr
    
    RpcServiceParam(IMsgFactory* factory,
        uint16_t max_handler_count = 4, 
        size_t max_call_out = 10000,
        size_t max_call_in = 10000,
        const char* ipc_addr = NULL, 
        const char* ip = NULL,
        uint16_t    port = 0)
    {
        this->msg_factory = factory;
        this->max_call_out = max_call_out;
        this->max_call_in = max_call_in;
        this->max_handler_count = max_handler_count;
        if (ipc_addr)
        {
            this->ipc_addr = ipc_addr;
        }
        if (ip)
        {
            this->ip = ip;
        }
        this->port = port;       
    }
    
    //!Add a new listen tcp addr
    void add_tcp_addr(const char* ip, uint16_t port)
    {
        tcp_addr_list.push_back(std::make_pair(ip, port));
    }
    
    //!Add a new listen ipc addr
    void add_ipc_addr(const char* ipc)
    {
        ipc_addr_list.push_back(ipc);
    }
    
public:
    uint16_t max_handler_count;
      
    //!param max_call_count Max concurrent rpc call num
    size_t max_call_out;

    //!param max_call_count Max concurrent rpc call num
    size_t max_call_in;
    
    std::string ip;
    uint16_t    port;
    
    std::string ipc_addr;
    
    
    IMsgFactory* msg_factory;
    
    std::vector<std::pair<std::string, uint16_t> > tcp_addr_list;
    std::vector<std::string> ipc_addr_list;      
};

class IRpcService
{
public:
    //!To initialize the service
 
    virtual int init(RpcServiceParam* param) = 0;
    
    virtual int start() = 0;
    virtual void stop() = 0;
    
    //!service_type will be binded to all listen tcp_addr and ipc_addr.
    virtual int set_stage_service(IStage* request_stage,
        int service_type) = 0;
    
    //!service_type will be binded to ip:port and ipc_addr.
    virtual int set_stage_service(IStage* request_stage,
        int service_type, 
        const char* ip, 
        unsigned short port,
        const char* ipc_addr) = 0;
        
    virtual int set_stage_service(IStage* request_stage,
        int service_type,
        std::vector<std::pair<std::string, uint16_t> >& tcp_addr_list,
        std::vector<std::string>& ipc_addr_list) = 0;       
                
    virtual int send_req(IStage* response_stage, IMsg* req, 
        int64_t transaction_id, int64_t trans_parm,
        const ServiceAddr& invoker, const ServiceAddr& responser,
        bool return_req, int timeout = 10) = 0;

    virtual int send_req(IStage* response_stage, IMsg* req, 
        int64_t transaction_id, int64_t trans_parm,
        const ServiceAddr& responser,
        int timeout = 10) = 0;
    
    //When call back handle_rpc_res, the req will be passed
    virtual int send_req(IStage* response_stage, IMsg* req,
        int64_t transaction_id, int64_t trans_parm,
        const ServiceAddr& responser, bool return_req,
        int timeout = 10) = 0;
            
    virtual int send_req(IStage* response_stage, IMsg* req, 
        int64_t trans_parm,
        const ServiceAddr& invoker, const ServiceAddr& responser, 
        int timeout = 10) = 0;
    
    virtual int send_req(IStage* response_stage, IMsg* req, 
        int64_t trans_parm,
        const ServiceAddr& responser,
        int timeout = 10) = 0;
    
                
    virtual int send_res(RpcHead* req_rpc_head, int res_code, IMsg* res) = 0;
    
    virtual ~IRpcService(){}
};


}//end namespace common
}//end namespace zonda

#endif //ZONDA_COMMON_IRPC_SERVICE_H_

