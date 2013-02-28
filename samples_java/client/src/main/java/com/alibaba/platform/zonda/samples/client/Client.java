package com.alibaba.platform.zonda.samples.client;


import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import com.alibaba.platform.zonda.common.rpc.*;
import com.alibaba.platform.zonda.common.seda.*;
import com.alibaba.platform.zonda.*;
import com.alibaba.platform.zonda.MsgFactory;

import com.alibaba.platform.zonda.msg.echo_service.*;
import com.alibaba.platform.zonda.logger.lib.AliLoggerFactory;
import com.alibaba.platform.zonda.common.config.zk.*;
import com.alibaba.platform.zonda.common.config.*;

import java.util.ArrayList;
import java.util.Vector;
import java.util.concurrent.atomic.*;
import java.util.Formatter;
import java.util.Locale;
import java.io.*;


class CallTime
{
    double sendTime;
    double recvTime;
}

class ConcurrentTestTask extends Thread
{
    private CallTime[] callTimeArray = new CallTime[Client.TEST_TIMES];
    public IRpcService rpc;
    private String testString;
    private ServiceAddr responser = new ServiceAddr();
    private Logger logger = AliLoggerFactory.getLogger("ConcurrentTestTask", Client.class);
    public ConcurrentTestTask()
    {
        StringBuilder sb = new StringBuilder();
        for (int i=0; i<1024;++i)
        {
            sb.append('a');
        }
        
        testString = sb.toString();        
        responser.serviceType.set(ServiceType.ECHO_SERVICE);
        
        for (int i=0; i<Client.TEST_TIMES; ++i)
        {
            callTimeArray[i] = new CallTime();
        }
    }
    public void run()
    {
        
        for (int i=0; i<Client.TEST_TIMES; ++i)
        {
            EchoReq req = new EchoReq();
            req.str = testString;            
            IMsg res;
            callTimeArray[i].sendTime = (System.nanoTime()/1000) / 1000000.0;
           
            try
            {
                //logger.debug("To send");
                res = rpc.call(req, responser, 1);
            }
            catch(Exception ex)
            {
                callTimeArray[i].recvTime = 0;
                logger.error(ex.getMessage());
                ex.printStackTrace();
                
            }
            callTimeArray[i].recvTime = (System.nanoTime()/1000) / 1000000.0;
        }      
        
        for (int i=0; i<Client.TEST_TIMES; ++i)
        {
            Client.callTimeVector.add(callTimeArray[i]);        
        }
               
        if (Client.aliveThreadNum.decrementAndGet() == 0)
        {
            Client.writeConcurrentTimeInfo();
        }
    }
}

public class Client
{
    public static final int TEST_TIMES = 100000;
    
    public static int successTimes = 0;
    public static int failedTimes = 0;
    public static double sendTime[] = new double[TEST_TIMES];
    public static double recvTime[] = new double[TEST_TIMES];
    
    public static Vector<CallTime> callTimeVector = new Vector<CallTime>();
    public static final int THREAD_NUM = 2;
    public static AtomicInteger aliveThreadNum = new AtomicInteger(THREAD_NUM);
    
    public static void writeTimeInfo()
    {
        FileWriter fw = null;
        try
        {
            fw = new FileWriter("/tmp/jizhao/java.txt"); 
            StringBuilder sb = new StringBuilder();
            Formatter formatter = new Formatter(sb, Locale.US);
            for (int i=0; i<TEST_TIMES; ++i)
            {
                //System.out.println("TEST_TIMES:" + TEST_TIMES);
                
                if (recvTime[i] == 0)
                {
                    //Skip error call
                    //System.out.println("Skip error call");
                    continue;
                }
                formatter.format("%06f\t%06f\t%06f\n", 
                    sendTime[i], recvTime[i], 
                    recvTime[i]-sendTime[i]);
                String s = sb.toString();
                fw.write(s, 0, s.length());
                sb.delete(0, sb.length());
            }
            fw.close();
        }
        catch(Exception e)
        {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
        finally
        {
            
        }
        
        System.exit(0);
    }

    public static void writeConcurrentTimeInfo()
    {
        System.out.println("writeConcurrentTimeInfo() ...");
        FileWriter fw = null;
        try
        {
            fw = new FileWriter("/tmp/jizhao/java.txt"); 
            StringBuilder sb = new StringBuilder();
            Formatter formatter = new Formatter(sb, Locale.US);
            for (int i=0; i<callTimeVector.size(); ++i)
            {
                //System.out.println("TEST_TIMES:" + TEST_TIMES);
                
                if (callTimeVector.get(i).recvTime == 0)
                {
                    //Skip error call
                    //System.out.println("Skip error call");
                    continue;
                }
                formatter.format("%06f\t%06f\t%06f\n", 
                    callTimeVector.get(i).sendTime, callTimeVector.get(i).recvTime, 
                    callTimeVector.get(i).recvTime-callTimeVector.get(i).sendTime);
                String s = sb.toString();
                fw.write(s, 0, s.length());
                sb.delete(0, sb.length());
            }
            fw.close();
        }
        catch(Exception e)
        {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
        finally
        {
            
        }
        
        System.exit(0);
    }    
    public static void BasicTest()
    {
        MsgFactory mf = new MsgFactory();
        GlobalServicesInfo info = GlobalServicesInfo.getInstance();
        int serviceCount = info.getServiceCount();              
        System.out.println("SERVICE_COUNT:" + serviceCount);
        for (int i=0; i<serviceCount; ++i)
        {
            System.out.println("ServiceType:" + i + ", " + info.getServiceName(i)); 
        }
        
        for (int i=101; i<100*serviceCount; ++i)
        {
            String s = info.getMsgName(i);
            if (s.length() != 0)
            {
                System.out.println("MsgType:" + i + ", " + s);
            }            
        }         
    }
    
    public static void normalUse()
    {
        Logger logger = AliLoggerFactory.getLogger("Client", Client.class);
        
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
            "com.alibaba.platform.zonda.samples.client.BizHandler",
            "BizStage");
        bizStage.init(null);

        
        IRpcService rpc = new RpcService();
        RpcServiceParam rpcParam = new RpcServiceParam();
        rpcParam.msgFactory = new MsgFactory();
        rpc.init(rpcParam);

        
        rpc.start();
        bizStage.start();
        
        
        //ArrayList<ServiceItem> serviceItemList = RouteTable.getInstance().getServiceItemList(1);
        
        //System.out.println(serviceItemList.size() + " " + serviceItemList.get(0).instanceId);
        
        try
        {
            
            ServiceAddr responser = new ServiceAddr();
            responser.serviceType.set(ServiceType.ECHO_SERVICE);
            for (int i=0 ;i<2; ++i)
            {
                EchoReq req = new EchoReq();
                req.str = "Hello " + i;            

                //rpc.sendReq(bizStage, req, 0, 0, responser);
                //rpc.sendReq(bizStage, req, 730024117138882561L);

                rpc.sendReq(bizStage, req, 0, 0, responser, true);              

            }
        }
        catch(Exception e)
        {
            logger.error("Failed to send req. Err:" + e.getMessage());
            e.printStackTrace();
        }
 
    }

    public static void testUse()
    {
        Logger logger = AliLoggerFactory.getLogger("Client", Client.class);
        
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
            "com.alibaba.platform.zonda.samples.client.TestBizHandler",
            "BizStage");
        bizStage.init(null);

        
        IRpcService rpc = new RpcService();
        RpcServiceParam rpcParam = new RpcServiceParam();
        rpcParam.msgFactory = new MsgFactory();
        rpc.init(rpcParam);

        
        rpc.start();
        bizStage.start();
        
        
        //ArrayList<ServiceItem> serviceItemList = RouteTable.getInstance().getServiceItemList(1);
        
        //System.out.println(serviceItemList.size() + " " + serviceItemList.get(0).instanceId);
        try
        {
            Thread.sleep(10*1000);
        }
        catch(Exception e)
        {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
                
        try
        {
            StringBuilder sb = new StringBuilder();
            for (int i=0; i<1024;++i)
            {
                sb.append('a');
            }
            String testString = sb.toString();
            ServiceAddr responser = new ServiceAddr();
            responser.serviceType.set(ServiceType.ECHO_SERVICE);
            for (int i=0 ;i<TEST_TIMES; ++i)
            {
                EchoReq req = new EchoReq();
                req.str = testString;            

                //rpc.sendReq(bizStage, req, 0, 0, responser);
                //rpc.sendReq(bizStage, req, 730024117138882561L);

                sendTime[i] = (System.nanoTime()/1000) / 1000000.0;
                rpc.sendReq(bizStage, req, i, i, responser, true);              
                
            }
        }
        catch(Exception e)
        {
            logger.error("Failed to send req. Err:" + e.getMessage());
            e.printStackTrace();
        }
 
    }
    
    public static void testSyncCall()
    {
        Logger logger = AliLoggerFactory.getLogger("Client", Client.class);
        
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
            "com.alibaba.platform.zonda.samples.client.TestBizHandler",
            "BizStage");
        bizStage.init(null);

        
        IRpcService rpc = new RpcService();
        RpcServiceParam rpcParam = new RpcServiceParam();
        rpcParam.msgFactory = new MsgFactory();
        rpc.init(rpcParam);

        
        rpc.start();
        bizStage.start();

        try
        {
            Thread.sleep(10*1000);
        }
        catch(Exception e)
        {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
                
        try
        {
            StringBuilder sb = new StringBuilder();
            for (int i=0; i<1024;++i)
            {
                sb.append('a');
            }
            String testString = sb.toString();
            ServiceAddr responser = new ServiceAddr();
            responser.serviceType.set(ServiceType.ECHO_SERVICE);
            for (int i=0; i<TEST_TIMES; ++i)
            {
                EchoReq req = new EchoReq();
                req.str = testString;            
                IMsg res;
                sendTime[i] = (System.nanoTime()/1000) / 1000000.0;

                
                try
                {
                    //logger.debug("To send");
                    res = rpc.call(req, responser, 1);
                }
                catch(Exception ex)
                {
                    recvTime[i] = 0;
                    //logger.error(ex.getMessage());
                    ex.printStackTrace();
                    
                }
                recvTime[i] = (System.nanoTime()/1000) / 1000000.0;
            }
            
            Client.writeTimeInfo();
        }
        catch(Exception e)
        {
            logger.error("Failed to send req. Err:" + e.getMessage());
            e.printStackTrace();
        }
 
    }
    
    public static void testSyncCallConcurrent()
    {
        Logger logger = AliLoggerFactory.getLogger("Client", Client.class);
        
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
            "com.alibaba.platform.zonda.samples.client.TestBizHandler",
            "BizStage");
        bizStage.init(null);

        
        IRpcService rpc = new RpcService();
        RpcServiceParam rpcParam = new RpcServiceParam();
        rpcParam.msgFactory = new MsgFactory();
        rpc.init(rpcParam);

        
        rpc.start();
        bizStage.start();
        try
        {
            Thread.sleep(10*1000);
        }
        catch(Exception e)
        {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
        
        ConcurrentTestTask[] testThreads = new ConcurrentTestTask[Client.THREAD_NUM];    
        for (int i=0; i<Client.THREAD_NUM; ++i)
        {
            testThreads[i] = new ConcurrentTestTask();
            testThreads[i].rpc = rpc;
        }
        
        for (int i=0; i<Client.THREAD_NUM; ++i)
        {
            testThreads[i].start();
        }       
        
    }
    
           
    public static void testLatency()
    {
        Logger logger = AliLoggerFactory.getLogger("Client", Client.class);
        
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
            "com.alibaba.platform.zonda.samples.client.TestLatencyBizHandler",
            "BizStage");
       

        
        IRpcService rpc = new RpcService();
        RpcServiceParam rpcParam = new RpcServiceParam();
        rpcParam.msgFactory = new MsgFactory();
        rpc.init(rpcParam);
        bizStage.init(rpc);

        
        rpc.start();
        bizStage.start();
        
        
        //ArrayList<ServiceItem> serviceItemList = RouteTable.getInstance().getServiceItemList(1);
        
        //System.out.println(serviceItemList.size() + " " + serviceItemList.get(0).instanceId);
        try
        {
            Thread.sleep(10*1000);
        }
        catch(Exception e)
        {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
                
        try
        {
            StringBuilder sb = new StringBuilder();
            for (int i=0; i<1024;++i)
            {
                sb.append('a');
            }
            String testString = sb.toString();
            ServiceAddr responser = new ServiceAddr();
            responser.serviceType.set(ServiceType.ECHO_SERVICE);
            for (int i=0 ;i<1; ++i)
            {
                EchoReq req = new EchoReq();
                req.str = testString;            

                //rpc.sendReq(bizStage, req, 0, 0, responser);
                //rpc.sendReq(bizStage, req, 730024117138882561L);
                sendTime[i] = (System.nanoTime()/1000) / 1000000.0;
                rpc.sendReq(bizStage, req, i, i, responser, true);              

            }
        }
        catch(Exception e)
        {
            logger.error("Failed to send req. Err:" + e.getMessage());
            e.printStackTrace();
        }
 
    }        
    public static void main(String args[])
    {
        //normalUse();
        //testUse();
        //testLatency();
        //testSyncCall();
        testSyncCallConcurrent();
    }
}