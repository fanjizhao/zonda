package com.alibaba.platform.zonda.common.rpc;

import com.alibaba.platform.zonda.common.seda.*;

public interface IRpcService
{
    void init(RpcServiceParam param);
    void start();
    void stop();
    void setStageService(IStage stage, int serviceType);
    void sendReq(IStage stage, IMsg req, long instanceId) throws Exception;
    void sendReq(IStage stage, IMsg req, long transParam, long transactionId) throws Exception;
    void sendReq(IStage stage, IMsg req, long instanceId, 
        long transParam, long transactionId) throws Exception;
    void sendReq(IStage stage, IMsg req,
        long transParam, long transactionId,
        ServiceAddr responser) throws Exception;
    void sendReq(IStage stage, IMsg req,
        long transParam, long transactionId,
        ServiceAddr responser, int timeout) throws Exception;
    void sendReq(IStage stage, IMsg req,
        long transParam, long transactionId,
        ServiceAddr responser, boolean returnReq) throws Exception;
    void sendReq(IStage stage, IMsg req,
        long transParam, long transactionId,
        ServiceAddr responser, boolean returnReq, int timeout) throws Exception;

    void sendRes(RpcHead reqRpcHead, int resCode, IMsg res) throws Exception;  
    
    
    IMsg call(IMsg req, ServiceAddr responser, int timeout) throws Exception;          
}


