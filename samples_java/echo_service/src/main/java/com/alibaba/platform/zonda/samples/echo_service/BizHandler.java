package com.alibaba.platform.zonda.samples.echo_service;

import com.alibaba.platform.zonda.common.rpc.*;
import com.alibaba.platform.zonda.common.seda.*;
import com.alibaba.platform.zonda.msg.echo_service.*;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import com.alibaba.platform.zonda.op_type.ECHO_SERVICE;

public class BizHandler extends RpcEventHandlerBase
{
    private Logger logger = LoggerFactory.getLogger("Client");
    private IRpcService rpc;
    public void init(IStage stage, Object param)
    {
        logger.debug("init is called");
        rpc = (IRpcService)param;
    }
    
    public void handleRpcReq(
        int opType,
        IMsg msg,
        RpcHead rpcHead)
    {
        logger.debug("Received req, opType:" + opType);
        
        if (opType == ECHO_SERVICE.ECHO)
        {
            EchoReq req = (EchoReq)msg;
            logger.debug("EchoReq, req.str:" + req.str);
            EchoRes res = new EchoRes();
            res.str = req.str;
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