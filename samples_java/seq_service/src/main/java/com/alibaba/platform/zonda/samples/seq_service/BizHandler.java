package com.alibaba.platform.zonda.samples.seq_service;

import com.alibaba.platform.zonda.common.rpc.*;
import com.alibaba.platform.zonda.common.seda.*;
import com.alibaba.platform.zonda.msg.seq_service.*;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import java.util.*;
import com.alibaba.platform.zonda.op_type.SEQ_SERVICE;

public class BizHandler extends RpcEventHandlerBase
{
    private Logger logger = LoggerFactory.getLogger("BizHandler");
    private IRpcService rpc;
    private HashMap<Short, Integer> m_seq_map = new HashMap<Short, Integer>();
    public void init(IStage stage, Object param)
    {
        logger.debug("init is called");
        rpc = (IRpcService)param;
    }
    
    public void handleEvent(IEvent event)
    {
        if (event.getType() == EventType.MASTER_SLAVE_SWITCH_EVENT)
        {
            MasterSlaveSwitchEvent e = (MasterSlaveSwitchEvent)event;
            logger.debug("Received MasterSlaveSwitchEvent, service_type:"
                + e.serviceType
                + ", old_role:" + e.oldRole
                + ", new_role:" + e.newRole);
            
            //TODO: actions for the role switching
        }
        else
        {
            super.handleEvent(event);
        }
        
        
    }
    
    public void handleRpcReq(
        int opType,
        IMsg msg,
        RpcHead rpcHead)
    {
        //logger.debug("Received req, opType:" + opType);
        
        if (opType == SEQ_SERVICE.GET_SEQ)
        {
            GetSeqReq req = (GetSeqReq)msg;
            logger.debug("GetSeqReq, req.seq_type:" + req.seq_type
                + ", count:" + req.count);
            GetSeqRes res = new GetSeqRes();
            Integer value = m_seq_map.get(req.seq_type);
            if (value == null)
            {
                value = new Integer(0);
                m_seq_map.put(req.seq_type, value);
            }

            res.start = value;
            res.count = req.count;
            m_seq_map.put(req.seq_type, value+res.count);
            
            try
            {
                rpc.sendRes(rpcHead, 0, res);
            }
            catch (Exception e)
            {
                logger.error("Failed to send res");
                e.printStackTrace();
            }
        }
        else
        {
            logger.error("Unknow Msg");
        }
    }

    public void handleRpcRes(
        int opType,
        int resCode,
        IMsg msg,
        long transParam,
        long transactionId,
        IMsg originalReq)
    {
    }
}