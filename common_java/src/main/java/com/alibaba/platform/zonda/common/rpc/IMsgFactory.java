package com.alibaba.platform.zonda.common.rpc;

public interface IMsgFactory
{
    byte[] serialize_body(IMsg msg) throws Exception;
    IMsg deserialize_body(byte[] buff, int pos, int len, int msgType) throws Exception;    
}
