package com.alibaba.platform.zonda.common.rpc;

import java.util.ArrayList;
import java.util.Set;

interface ITransReceiver
{
    void init(Object parm);
    void start();
    void stop();
    void getListenAddr(
        ArrayList<IpAddr> tcpAddrList,
        ArrayList<String> ipcAddrList);
    
    void getListenAddr(
        Set<IpAddr> tcpAddrSet,
        Set<String> ipcAddrSet);
}