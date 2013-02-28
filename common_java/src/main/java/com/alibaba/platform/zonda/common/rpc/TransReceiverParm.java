package com.alibaba.platform.zonda.common.rpc;


import java.util.ArrayList;
import com.alibaba.platform.zonda.common.seda.*;

public class TransReceiverParm
{
    public TransReceiverParm()
    {
        startPort = 5000;
        portCount = 1000;
        autoListenTcp = true;
    }
    
    ArrayList<IpAddr> tcpAddrList = new ArrayList<IpAddr>();
    ArrayList<String> ipcAddrList = new ArrayList<String>();

    //!When the trans received a data, it will remove
    //the corresponding record if the msg is an res
    ArrayList<CallOutRecord> callOutRecordList = new ArrayList<CallOutRecord>();
    int startPort;
    int portCount;
    boolean autoListenTcp;
    IMsgFactory msgFactory;
};