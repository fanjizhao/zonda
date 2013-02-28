package com.alibaba.platform.zonda.common.rpc;

import com.alibaba.platform.zonda.common.seda.*;

public class RpcEvent implements IEvent
{
    private IMsg m_msg;
    private RpcHead m_rpc_head;
    private int m_msg_type;
    private IMsg m_req;
    public RpcEvent(IMsg msg, RpcHead rh)
    {
        m_msg_type = msg.getMsgType();
        m_msg = msg;
        m_rpc_head = rh;
        m_req = null;
    }
    
    public RpcEvent(RpcHead rh)
    {
        m_msg_type = rh.msgType.get();
        m_msg = null;
        m_rpc_head = rh;
        m_req = null;
    }
    
    public int getType()
    {
        return EventType.RPC_EVENT;
    }
    
    public boolean isReq()
    {
        if (m_msg_type > 0) return true;
        return false;
    }
    
    public boolean isRes()
    {
        if (m_msg_type > 0) return false;
        return true;        
    }
    
    public int getOpType()
    {
        if (m_msg_type > 0) return m_msg_type;
        return 0-m_msg_type;
    }
    
    public IMsg getMsg()
    {
        return m_msg;
    }
    
    public RpcHead getRpcHead()
    {
        return m_rpc_head;
    }
    
    //!Only for res event
    public void setReq(IMsg req)
    {
        m_req = req;
    }
    
    //!Only for res event
    public IMsg getReq()
    {
        return m_req;
    }
}