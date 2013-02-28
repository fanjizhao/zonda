#ifndef ZONDA_COMMON_SYNC_RPC_SERVICE_H_
#define ZONDA_COMMON_SYNC_RPC_SERVICE_H_

#include "IMsg.h"
#include "thread/ThreadLock.h"
#include "thread/ThreadCond.h"
#include "RpcEventHandlerBase.h"

namespace zonda
{
namespace common
{
    
class SyncAsyncConvertItem
{
public:
    SyncAsyncConvertItem();
    ThreadLock lock;
    ThreadCond cond;
    IMsg* res;
    int res_code;
};

class SyncRpcHandler: public RpcEventHandlerBase
{
public:
    SyncRpcHandler();
    virtual int init(zonda::common::IStage* stage, void* param);
    virtual int handle_event(const zonda::common::IEvent* event);
    virtual ~SyncRpcHandler() {};
    
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
};

class SyncRpcService
{
public:
    SyncRpcService(IRpcService* async_rpc);
    //! After the call, req will be unaccessible becaue it's freed in the call
    int call(IMsg* req, IMsg* &res, const ServiceAddr& responser);
    int start();
    void stop();
private:
    IRpcService* m_rpc;
    IStage* m_stage;
};

} //end namespace common
} //end namespace zonda
#endif //ZONDA_COMMON_SYNC_RPC_SERVICE_H_

