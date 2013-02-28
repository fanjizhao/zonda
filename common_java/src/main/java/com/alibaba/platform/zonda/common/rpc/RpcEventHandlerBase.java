package com.alibaba.platform.zonda.common.rpc;

import com.alibaba.platform.zonda.common.seda.*;

public abstract class RpcEventHandlerBase implements IEventHandler
{
    //! When an rpc request arrives, the method is called
    public void handleRpcReq(
        int opType,
        IMsg msg,
        RpcHead rpcHead)
    {
    }
     
    public abstract void handleRpcRes(
        int opType,
        int resCode,
        IMsg msg,
        long transParam,
        long transactionId,
        IMsg originalReq);
    
    public void handleEvent(IEvent event)
    {
        if (event.getType() != EventType.RPC_EVENT)
        {
            throw new RuntimeException("Unknow EventType:" + event.getType());
        }
        RpcEvent e = (RpcEvent)event;
        RpcHead rpcHead = e.getRpcHead();
        if (e.isReq())
        {
            handleRpcReq(e.getOpType(),
                    e.getMsg(),
                    rpcHead);
        }
        else
        {
            
            handleRpcRes(
                e.getOpType(),
                rpcHead.resCode.get(),
                e.getMsg(),
                rpcHead.transParam.get(),
                rpcHead.transactionId.get(),
                e.getReq());
        }
    
    }    
}