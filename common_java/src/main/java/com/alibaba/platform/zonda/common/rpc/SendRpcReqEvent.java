package com.alibaba.platform.zonda.common.rpc;

import com.alibaba.platform.zonda.common.seda.*;

public class SendRpcReqEvent implements IEvent
{

    public SendRpcReqEvent()
    {
    }
    public int getType()
    {
        return EventType.SEND_RPC_REQ_EVENT;
    }
    
    public IMsg     msg = null;
    public RpcHead rpcHead;
    public CallOutRecordItem callOutRecordItem;
}

