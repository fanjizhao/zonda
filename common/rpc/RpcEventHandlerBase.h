#ifndef ZONDA_COMMON_IRPC_EVENT_HANDLER_H_
#define ZONDA_COMMON_IRPC_EVENT_HANDLER_H_

#include "seda/IEventHandler.h"
#include "IMsg.h"
#include "RpcService.h"
#include "RpcEvent.h"

namespace zonda
{
namespace common
{

//!If a stage needs handle rpc request/response, its handler ought to inherit this class.
struct RpcEventHandlerBase: public IEventHandler
{
    //! When an rpc request arrives, the method is called
    virtual int handle_rpc_req(
        int op_type, 
        IMsg* msg, 
        RpcHead* rpc_head) = 0;
    
    //! When an rpc response comes back, the method is called   
    virtual int handle_rpc_res(int op_type,
            int res_code,
            IMsg *msg,
            int64_t trans_param,
            int64_t transaction_id,
            IMsg* original_req) = 0;
           
    int handle_rpc_event(const IEvent* event);            
};



} //namespace server_platform
} //namespace zonda

#endif //ZONDA_COMMON_IRPC_EVENT_HANDLER_H_
