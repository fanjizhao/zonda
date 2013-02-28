
#include "RpcEvent.h"


using namespace zonda::common;

RpcEvent::RpcEvent(IMsg* msg, RpcHead* head):
    m_msg(msg), m_rpc_head(head), m_req(NULL)
{
    m_msg_type = m_rpc_head->msg_type;
}

RpcEvent::RpcEvent(RpcHead* head):
    m_msg(NULL), m_rpc_head(head)
{
    m_msg_type = m_rpc_head->msg_type;
}


RpcEvent::~RpcEvent()
{
    //NOTE:msg will be deleted by the user
    
    if (m_msg_type < 0)
    {
        delete m_rpc_head;
    }

}

int RpcEvent::get_type() const
{
    return EventType::RPC_EVENT;
}

bool RpcEvent::is_req() const
{
    if (m_msg_type > 0) return true;
    else return false;
}

bool RpcEvent::is_res() const
{
    if (m_msg_type > 0) return false;
    else return true;

}

int RpcEvent::get_op_type() const
{
    if (m_msg_type > 0) return m_msg_type;
    else return (-1*m_msg_type);
}

  
IMsg* RpcEvent::get_msg() const
{
    return m_msg;
}

RpcHead* RpcEvent::get_rpc_head() const
{
    return m_rpc_head;
}

//!Only for res event
void RpcEvent::set_req(IMsg* req)
{
    m_req = req;
}
//!Only for res event
IMsg* RpcEvent::get_req()
{
    return m_req;
}


