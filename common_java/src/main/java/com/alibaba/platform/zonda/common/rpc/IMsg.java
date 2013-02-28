package com.alibaba.platform.zonda.common.rpc;

public interface IMsg 
{
    int getMsgType();
    int getOpType();
    byte[] serialize() throws Exception;
    void deserialize(byte[] buff, int pos, int len) throws Exception;
}
