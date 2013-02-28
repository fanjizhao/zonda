package com.alibaba.platform.zonda.common.rpc;

public class IpAddr
{
    public IpAddr(String ipStr, int portNum)
    {
        ip = ipStr;
        port = portNum;
    }
    
    public IpAddr() {}
    public String toString()
    {
        return ip + ":" + port;
    }
    public String ip;
    public int port;
}
