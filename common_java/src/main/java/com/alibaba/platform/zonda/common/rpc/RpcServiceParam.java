package com.alibaba.platform.zonda.common.rpc;

import com.alibaba.platform.zonda.common.seda.*;
import java.util.ArrayList;

public class RpcServiceParam
{
    public RpcServiceParam()
    {
        maxHandlerCount = 4;
        maxCallOut = 10000;
        maxCallIn = 10000;
    }
    public int maxHandlerCount;

    //!param max_call_count Max concurrent rpc call num
    public int maxCallOut;

    //!param max_call_count Max concurrent rpc call num
    public int maxCallIn;

    public String ip;
    public int port;

    public String ipcAddr;


    public IMsgFactory msgFactory;

    ArrayList<IpAddr> tcpAddrList = new ArrayList<IpAddr>();
    ArrayList<String> ipcAddrList = new ArrayList<String>();

}