

#include "RpcEventHandlerBase.h"

using namespace zonda::common;
int RpcEventHandlerBase::handle_rpc_event(const IEvent* event)
{
    RpcEvent* e = (RpcEvent*)event;
    int r=0;
    RpcHead* rpc_head = e->get_rpc_head();
    if (e->is_req())
    {
        r = handle_rpc_req(e->get_op_type(),
                e->get_msg(),
                rpc_head);
    }
    else
    {
        r = handle_rpc_res(
                e->get_op_type(),
                rpc_head->res_code,
                e->get_msg(),
                rpc_head->trans_param,
                rpc_head->transaction_id,
                e->get_req());
    }
    
    delete e;
    return r;
}




