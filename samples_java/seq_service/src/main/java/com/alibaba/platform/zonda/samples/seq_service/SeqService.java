package com.alibaba.platform.zonda.samples.seq_service;


import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import com.alibaba.platform.zonda.common.rpc.*;
import com.alibaba.platform.zonda.common.seda.*;
import com.alibaba.platform.zonda.common.config.zk.*;
import com.alibaba.platform.zonda.common.config.*;
import com.alibaba.platform.zonda.logger.lib.AliLoggerFactory;

import com.alibaba.platform.zonda.*;
import com.alibaba.platform.zonda.MsgFactory;

import com.alibaba.platform.zonda.msg.seq_service.*;

import java.util.ArrayList;

public class SeqService
{
    public static void main(String args[])
    {
        Logger logger = AliLoggerFactory.getLogger("SeqService", SeqService.class);
        
        MetaInfo meta = new MetaInfo();
        //connectString - comma separated host:port pairs, 
        //each corresponding to a ZooKeeper server. 
        meta.setConnectString("127.0.0.1:2181");
        //session timeout in milliseconds
        meta.setSessionTimeout(6*1000);
        Configer config = ZkConfiger.getInstance();
        try
        {
            config.initialize(meta, null);
        }
        catch(Exception e)
        {
            logger.error(e.getMessage());
            e.printStackTrace();
            System.exit(0);                        
        }
        
        IStage bizStage = new Stage(
            "com.alibaba.platform.zonda.samples.seq_service.BizHandler",
            "BizStage", 10000, 1, StageThreadMode.EXLUSIVE);
 

        
        IRpcService rpc = new RpcService();
        bizStage.init(rpc);
        
        RpcServiceParam rpcParam = new RpcServiceParam();
        rpcParam.msgFactory = new MsgFactory();
        rpc.init(rpcParam);
        rpc.setStageService(bizStage, ServiceType.SEQ_SERVICE);

        
        rpc.start();
        bizStage.start();
    }
}
