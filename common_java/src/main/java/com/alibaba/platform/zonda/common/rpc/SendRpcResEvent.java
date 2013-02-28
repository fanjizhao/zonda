package com.alibaba.platform.zonda.common.rpc;

import com.alibaba.platform.zonda.common.seda.*;

public class SendRpcResEvent implements IEvent
{
    public SendRpcResEvent(){}
    public int getType()
    {
        return EventType.SEND_RPC_RES_EVENT;
    }
    public RpcHead reqRpcHead;
    public int resCode;
    public IMsg msg;
};
