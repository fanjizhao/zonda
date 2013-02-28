package com.alibaba.platform.zonda.samples.client;


import com.alibaba.platform.zonda.common.rpc.*;
import com.alibaba.platform.zonda.common.seda.*;
import com.alibaba.platform.zonda.MsgFactory;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.alibaba.platform.zonda.op_type.ECHO_SERVICE;
import com.alibaba.platform.zonda.msg.echo_service.*;
import com.alibaba.platform.zonda.logger.lib.AliLoggerFactory;
import com.alibaba.platform.zonda.*;


public class TestLatencyBizHandler extends RpcEventHandlerBase
{
    private Logger logger = AliLoggerFactory.getLogger("client", getClass());
    private IStage selfStage;
    private ServiceAddr responser = new ServiceAddr();
    private IRpcService rpc;
    public void init(IStage stage, Object param)
    {
        logger.debug("init is called");
        selfStage = stage;
        responser.serviceType.set(ServiceType.ECHO_SERVICE);
        rpc = (IRpcService)param;
    }
    
    public void handleRpcRes(
        int opType,
        int resCode,
        IMsg msg,
        long transParam,
        long transactionId,
        IMsg originalReq)
    {
//        logger.debug("opType:" + opType + ", resCode:" + resCode
//            + ", transParam:" + transParam
//            + ", transactionId:" + transactionId);
        
        switch (opType)
        {
            case ECHO_SERVICE.ECHO:
                if (resCode == 0)
                {
                    Client.recvTime[(int)transactionId] = (System.nanoTime()/1000) / 1000000.0;
                    Client.successTimes++;
                    EchoRes res = (EchoRes)msg;
                    //logger.debug("EchoRes, str:" + res.str);

                }
                else
                {
                    Client.failedTimes++;
                    Client.recvTime[(int)transactionId] = 0;
                }
                
                if (Client.successTimes + Client.failedTimes == Client.TEST_TIMES)
                {
                    logger.error("successTimes:" + Client.successTimes
                        + ", failedTimes:" + Client.failedTimes);
                    Client.writeTimeInfo();
                    System.exit(0);
                }
                
                EchoReq req = new EchoReq();
                req.str = ((EchoRes)msg).str;
                try
                {
                    int i=(int)(transParam+1);
                    rpc.sendReq(selfStage, req, i, i, responser, true);
                    Client.sendTime[i] = (System.nanoTime()/1000) / 1000000.0;
                }
                catch(Exception e)
                {
                    logger.error("Failed to send req");
                    e.printStackTrace();
                }                
                break;
            default:
                logger.error("Unknow Msg");
                break;
        }
        
    }
}