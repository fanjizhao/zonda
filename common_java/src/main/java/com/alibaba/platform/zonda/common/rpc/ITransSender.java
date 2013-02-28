package com.alibaba.platform.zonda.common.rpc;

interface ITransSender
{
    void init(Object param);
    void sendData(long callId, byte[] buff, String ip, int port) throws Exception;
}


