#ifndef ZONDA_COMMON_RPC_EVENT_H_
#define ZONDA_COMMON_RPC_EVENT_H_

#include "seda/IEvent.h"
#include "seda/EventType.h"
#include "IMsg.h"
#include "RpcService.h"

namespace zonda
{
namespace common
{


class RpcEvent: public IEvent
{
public:
    RpcEvent(IMsg* msg, RpcHead* head);
    
    //! For constructing an error response
    RpcEvent(RpcHead* head);
    ~RpcEvent();
    virtual int get_type() const;
    bool is_req() const;
    bool is_res() const;
    //! Get the operation type
    int get_op_type() const;
    //! Get pointer to the real req/res 
    IMsg* get_msg() const;
    RpcHead* get_rpc_head() const;
    //!Only for res event
    void set_req(IMsg* req);
    //!Only for res event
    IMsg* get_req();

private:
    IMsg* m_msg;
    RpcHead* m_rpc_head;
    int m_msg_type;
    IMsg* m_req;
};


}//namespace common
}//namespace zonda




#endif //ZONDA_COMMON_RPC_EVENT_H_


