package com.alibaba.platform.zonda.common.rpc;

import java.util.ArrayList;
import java.util.Set;
import java.util.HashSet;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.zeromq.ZMQ;
import org.zeromq.ZMQ.Socket;
import com.alibaba.platform.zonda.common.util.NetUtil;
import com.alibaba.platform.zonda.common.seda.*;
import com.alibaba.platform.zonda.logger.lib.AliLoggerFactory;
import java.util.concurrent.locks.ReentrantLock;

public class ZMQReceiver implements ITransReceiver, Runnable
{
    private Logger logger = AliLoggerFactory.getLogger(Constants.MODULE_NAME, getClass());
    private ZMQ.Context m_zmq_context;
    private ZMQ.Poller m_items;
    private ArrayList<ZMQ.Socket> m_zmq_socket_list = new  ArrayList<ZMQ.Socket>();
    private ArrayList<IpAddr> m_tcp_addr_list = new ArrayList<IpAddr>();
    private boolean m_stop = false;
    private boolean m_start = false;
    private IMsgFactory m_msg_factory;
    private ArrayList<CallOutRecord>   m_call_out_record_list = new ArrayList<CallOutRecord>();
    private ReentrantLock m_lock = new ReentrantLock();
    private int transHeadSize;

    private void doAutoTcpListen(int startPort, int portCount)
    {
        ZMQ.Socket zmq_sock = null;
    
        zmq_sock = m_zmq_context.socket(ZMQ.PULL);

        for (int port = startPort; port < startPort+portCount; ++port)
        {
            String listenAddr = "tcp://0.0.0.0:" + port;
            logger.debug("zmq try to bind to " + listenAddr);
            try
            {
                zmq_sock.bind(listenAddr);
            }
            catch (Exception e)
            {
                logger.debug("Failed to call zmq_bind("  + listenAddr
                    + e.getMessage());
                continue;
            }

            logger.info("zmq bind to " + listenAddr);
            m_zmq_socket_list.add(zmq_sock);

            HashSet<String> ip_set = new HashSet<String>();
            NetUtil.getLocalIpAddr(ip_set);
            for (String ip: ip_set)
            {
                IpAddr addr = new IpAddr(ip, port);
                m_tcp_addr_list.add(addr);
                logger.debug("add " + addr.toString() + " into local tcp listen addr");
            }
            return;
        }
    
    }
    
    private void handleIncomingMsg(RpcHead rpcHead, IMsg msg)
    {
        logger.debug("Begin to handle a msg, RpcHead, msg_type:" + rpcHead.msgType
            + ", call_id:" + rpcHead.callId
            +  ", transaction_id:" + rpcHead.transactionId
            + ", responser_type:" + rpcHead.responser.serviceType
            + ", invoker_type:" + rpcHead.invoker.serviceType
            + ", biz_handler_id:" + rpcHead.bizHandlerId);
        
        CallOutRecordItem item = null;
        IStage bizStage = null;

        //Handle req msg
        if (rpcHead.msgType.get() > 0)
        {
            //Req
            bizStage = ServiceStageMap.getInstance().getStage(rpcHead.responser.serviceType.get());
            
            if( bizStage == null)
            {
                logger.error("Unsupported serviceType:" + rpcHead.responser.serviceType
                    +", msgType: " + rpcHead.msgType);
                return;
            }
            logger.debug("Req target stage is:" + bizStage.getName());

            RpcEvent event = new RpcEvent(msg, rpcHead);
            bizStage.getSink().enqueue(event);
            
            return;

        }

        //Handle res msg
        //To remove the call out record
        int targetCallOutRecord = (int)(rpcHead.callId.get() % m_call_out_record_list.size());
        try
        {
            item = m_call_out_record_list.get(targetCallOutRecord).remove(rpcHead.callId.get());
        }
        catch (Exception e)
        {
            logger.error("call_id:" + rpcHead.callId.get()
                + ", does not exist in call_out_record. "
                + "It might timeout.");
            return;
        }  
        
        logger.debug("call_id:" + rpcHead.callId.get()
            + ", sync:" + item.sync);
        if (item.sync == true)
        {
            if (item.timeoutFlag == true)
            {
                //The call thread has timed out
                //so don't need do anything
            }
            else
            {
                logger.debug("call_id:" + rpcHead.callId.get()
                    + " is sync ,to notify");
                item.res = msg;
                item.notify_res();                    
            }
            return;
        }

        bizStage = StageMgr.getInstance().getStage(rpcHead.bizHandlerId.get());
        logger.debug("Res target stage is:" + bizStage.getName());
       
        RpcEvent event = new RpcEvent(msg, rpcHead);
        event.setReq(item.req);
        bizStage.getSink().enqueue(event);
        
        
    }
      
    public void init(Object param)
    {
        if (m_start) return;
        m_zmq_context = ZMQ.context(1);
        TransReceiverParm p = (TransReceiverParm)param;
        doAutoTcpListen(5000, 1000);
        m_msg_factory = p.msgFactory;
        m_call_out_record_list.addAll(p.callOutRecordList);
        
        m_items = m_zmq_context.poller(m_zmq_socket_list.size());
        for (ZMQ.Socket s : m_zmq_socket_list)
        {
            m_items.register(s, ZMQ.Poller.POLLIN);
        }        
             
        m_start = true;
        
        TransHead th = new TransHead();
        transHeadSize = th.size();
        logger.debug("TransHead size:" + transHeadSize);
    }
    
    public void run()
    {
        m_lock.lock();
        while (!m_stop)
        {
            byte[] data;
            m_items.poll();
            
            for (int i=0; i<m_zmq_socket_list.size(); ++i)
            {
                if (m_items.pollin(i))
                {
                    data = m_items.getSocket(i).recv(0);
                    if (data.length < transHeadSize)
                    {
                        //skip 
                        continue;
                    }
                    
                    m_lock.unlock();
                    
                    TransHead th = new TransHead();
                    th.deserialize(data);
                    
                    logger.debug("Received msg, len:" + data.length
                        + ", version:" + th.version
                        + ", rpcHeadLen: " + th.rpcHeadLen
                        + ", rpcMsgLen: " + th.rpcMsgLen);
                        
                    RpcHead rh = new RpcHead();
                    IMsg msg = null;
                    
                    if (transHeadSize + th.rpcHeadLen.get() + th.rpcMsgLen.get() != data.length)
                    {
                        logger.error("Invalid msg");
                        m_lock.lock();
                        continue;
                    }
                    try
                    {
                        rh.deserialize(data, th.size(), rh.size());
                        msg = m_msg_factory.deserialize_body(data,  th.size() + rh.size(), th.rpcMsgLen.get(), rh.msgType.get());
                        logger.debug("Received a msg, RpcHead, msg_type:" + rh.msgType
                            + ", call_id:" + rh.callId
                            + ",tranaction_id:" + rh.transactionId
                            + ", invoker_type:" + rh.invoker.serviceType
                            + ", invoker.instance_id:" + rh.invoker.instanceId
                            + ", responser_type:" + rh.responser.serviceType
                            + ", trans_param:" + rh.transParam);                        
                        handleIncomingMsg(rh, msg);
                    }
                    catch(Exception e)
                    {
                        logger.warn("Failed to deserialize msg. Err:" + e.getMessage());
                        e.printStackTrace();
                        m_lock.lock();
                        continue;
                    }
                    m_lock.lock();
                }
            }// end for            
        }//end while
    }
    
    public void start()
    {
        for (int i=0; i<m_call_out_record_list.size(); ++i)
        {
            Thread t = new Thread(this, "ZMQReceiver");
            t.start();
        }
    }
    public void stop()
    {
        m_stop = true;
    }
    public void getListenAddr(
        ArrayList<IpAddr> tcpAddrList,
        ArrayList<String> ipcAddrList)
    {
        tcpAddrList.addAll(m_tcp_addr_list);
    }
    
    public void getListenAddr(
        Set<IpAddr> tcpAddrSet,
        Set<String> ipcAddrSet)
    {
        tcpAddrSet.addAll(m_tcp_addr_list);
    }
}
