package com.alibaba.platform.zonda.common.rpc;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.zeromq.ZMQ;
import org.zeromq.ZMQ.Socket;
import java.util.HashMap;
import com.alibaba.platform.zonda.logger.lib.AliLoggerFactory;


public class ZMQSender implements ITransSender
{
    private Logger logger = AliLoggerFactory.getLogger(Constants.MODULE_NAME, getClass());
    private ZMQ.Context m_zmq_context;
    private HashMap<Long, ZMQ.Socket> m_tcp_conn_map = new HashMap<Long, ZMQ.Socket>();
    public void init(Object param)
    {
        m_zmq_context = ZMQ.context(1);
        logger.debug("init() called");
    }
    
    private static long makeChannelId(String ip, int port)
    {
        String[] addrArray = ip.split("\\.");
        long num = 0;
        for (int i=0;i<addrArray.length;i++) 
        {
            int power = 3-i;
            num += ((Integer.parseInt(addrArray[i])%256 * Math.pow(256,power)));
        }
        
        long channelId = (num << 32) + port;
        
        return channelId;
    }
    
    private ZMQ.Socket getConn(String ip, int port) throws Exception
    {
        Long channelId  = makeChannelId(ip, port);
        ZMQ.Socket s = m_tcp_conn_map.get(channelId);
        if (s != null) 
        {
            logger.debug("Get connection to " + ip + ":" + port
                + " from the map");            
            return s;
        }
        
        s = m_zmq_context.socket(ZMQ.PUSH);
        String addr = "tcp://" + ip + ":" + port;
        s.connect(addr);
        logger.debug("Successfully connected to: " + addr);
        m_tcp_conn_map.put(channelId, s);
        
        return s;
    }
    
    private void removeConn(String ip, int port)
    {
        Long channelId  = makeChannelId(ip, port);
        m_tcp_conn_map.remove(channelId);
        logger.debug("Remove connection to " + ip + ":" + port);
    }
    
    public void sendData(long callId, byte[] buff, String ip, int port) throws Exception
    {
        ZMQ.Socket socket = getConn(ip, port);
        if (socket.send(buff, 0) != true)
        {
            String err="Failed to send data";
            logger.error(err);
            removeConn(ip, port);
            throw new Exception(err);
        }
    }
    
    public static void main(String args[])
    {
        long id = makeChannelId("1.2.3.4", 65534);
        System.out.println(Long.toHexString(id));
    }
}
