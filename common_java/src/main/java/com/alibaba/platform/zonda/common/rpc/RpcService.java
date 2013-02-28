package com.alibaba.platform.zonda.common.rpc;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import com.alibaba.platform.zonda.common.seda.*;
import java.util.HashMap;
import java.util.ArrayList;
import java.nio.ByteBuffer;
import java.util.concurrent.atomic.AtomicLong; 



import com.alibaba.platform.zonda.common.config.Configer;
import com.alibaba.platform.zonda.common.config.CreateMode;
import com.alibaba.platform.zonda.common.config.MetaInfo;
import com.alibaba.platform.zonda.common.config.exception.ConfigUnintializedException;
import com.alibaba.platform.zonda.common.config.exception.CreateNodeFailedException;
import com.alibaba.platform.zonda.common.config.exception.SetValueFailedException;
import com.alibaba.platform.zonda.common.config.zk.ZkConfiger;
import com.alibaba.platform.zonda.logger.lib.AliLoggerFactory;


   
public class RpcService implements IRpcService
{
    private Logger logger = AliLoggerFactory.getLogger(Constants.MODULE_NAME, getClass());
    private ArrayList<IStage> m_rpc_stage_list = new ArrayList<IStage>();
    private IEventSink     m_default_rpc_sink;
    private ZMQReceiver m_trans_receiver = new ZMQReceiver();
    //TimerService m_timer;
    private AtomicLong m_call_id = new AtomicLong();
    private int m_max_call_out;
    private int m_max_call_in;
    private int m_handler_count;
    private Configer config = null;
    
    public void init(RpcServiceParam param)
    {
        m_handler_count = param.maxHandlerCount;
        m_max_call_out = param.maxCallOut;
        m_max_call_in = param.maxCallIn;
        
        if (m_handler_count == 0)
        {
            String err = "maxHandlerCount must not be zero!!";
            logger.error(err);
            throw new RuntimeException(err);
        }
        
        RpcStageHandlerParam stage_param = new RpcStageHandlerParam();
        stage_param.rpcParam = param;
        
        TransReceiverParm trans_param = new TransReceiverParm(); 
        
        IStage rpc_stage = null;
        int queue_capacity = (2*m_max_call_out + m_max_call_in )/m_handler_count;
        for (int i=0; i<m_handler_count; ++i)
        {
            stage_param.callOutRecord = new CallOutRecord();
            trans_param.callOutRecordList.add(stage_param.callOutRecord);
            String buff = "rpc_stage_" +  i;
            rpc_stage = new Stage("com.alibaba.platform.zonda.common.rpc.RpcStageHander", 
                buff, queue_capacity, 1, StageThreadMode.EXLUSIVE);
            rpc_stage.init(stage_param);
            m_rpc_stage_list.add(rpc_stage);
        }
        
            
    
        m_default_rpc_sink = m_rpc_stage_list.get(0).getSink();
       
        
    
        if (param.ip == null)
        {
            IpAddr ipAddr = new IpAddr(param.ip, param.port);
            param.tcpAddrList.add(ipAddr);
        }
        
        if (param.ipcAddr == null)
        {
            param.ipcAddrList.add(param.ipcAddr);
        }
           
    
   
        trans_param.tcpAddrList.addAll(param.tcpAddrList);
        trans_param.ipcAddrList.addAll(param.ipcAddrList);
        trans_param.msgFactory = param.msgFactory;

        m_trans_receiver.init(trans_param);
      
    
        ArrayList<IpAddr> tcpAddrList = new ArrayList<IpAddr>();
        ArrayList<String> ipcAddrList = new ArrayList<String>();
        
        m_trans_receiver.getListenAddr(tcpAddrList, ipcAddrList);
        RouteTable.getInstance().setLocalListenAddr(tcpAddrList, ipcAddrList);
        ServiceStageMap.getInstance().setLocalListenAddr(tcpAddrList);
        
        
        //Load route strategy info
        config = ZkConfiger.getInstance();
		try
		{

            byte[] data;
            data = config.getValue(Configer.ZONDA_ROUTE_STRATEGY_PATH, 
                m_default_rpc_sink, true);
            
            RouteStrategy.getInstance().load(new String(data));
            
            
            //Load service grouping info
            data = config.getValue(Configer.ZONDA_SERVICE_GROUPING_PATH,
                m_default_rpc_sink, true);
            ServiceGroupingTable.getInstance().load(new String(data));
		
            logger.debug("To set watch on Config::ZONDA_ROUTE_TABLE_PATH");
            byte[] route_table_data = config.getValue(Configer.ZONDA_ROUTE_TABLE_PATH,
                m_default_rpc_sink, true);
            RouteTable.getInstance().load(new String(route_table_data));  
            		
		}
		catch(Exception e)
		{
		    e.printStackTrace();
		    throw new RuntimeException(e.getMessage());
		}        

        m_call_id.set(System.currentTimeMillis()*1000);
    }
    
    public void start()
    {
        for (int i=0; i<m_rpc_stage_list.size(); ++i)
        {
            m_rpc_stage_list.get(i).start();
        }    
        m_trans_receiver.start();
    }
    
    public void stop()
    {
        for (int i=0; i<m_rpc_stage_list.size(); ++i)
        {
            m_rpc_stage_list.get(i).stop();
        }    
        m_trans_receiver.stop();        
    }
    
    public void setStageService(IStage stage, int serviceType)
    {
        ArrayList<IpAddr> tcpAddrList = new ArrayList<IpAddr>();
        ArrayList<String> ipcAddrList = new ArrayList<String>();
        
        m_trans_receiver.getListenAddr(tcpAddrList, ipcAddrList);
        
           
        ServiceStageMap.getInstance().setStageService(
            stage, serviceType, tcpAddrList, ipcAddrList);
        
    }
    
    //direct
    public void sendReq(IStage stage, IMsg req, long instanceId) throws Exception
    {
        ServiceAddr responser = new ServiceAddr();
        int tmp = req.getMsgType() % 100;
        int serviceType = (req.getMsgType() - tmp) / 100;
        responser.serviceType.set(serviceType);        
        responser.instanceId.set(instanceId);
        sendReq(stage, req, 0, 0, responser); 
    }
    
    //only for localhost
    public void sendReq(IStage stage, IMsg req, long transParam, 
        long transactionId) throws Exception
    {
        ServiceAddr responser = new ServiceAddr();
        int tmp = req.getMsgType() % 100;
        int serviceType = (req.getMsgType() - tmp) / 100;
        responser.serviceType.set(serviceType);
        sendReq(stage, req, transParam,transactionId, responser); 
    }
    
    public void sendReq(IStage stage, IMsg req, long instanceId, 
        long transParam, long transactionId) throws Exception
    {
        ServiceAddr responser = new ServiceAddr();
        responser.instanceId.set(instanceId);
        int tmp = req.getMsgType() % 100;
        int serviceType = (req.getMsgType() - tmp) / 100;
        responser.serviceType.set(serviceType);       
        sendReq(stage, req, transParam,transactionId, responser); 
    }
    
    public void sendReq(IStage stage, IMsg req,
        long transParam, long transactionId,
        ServiceAddr responser) throws Exception
    {
        if (req == null)
        {
            String err="req is null";
            logger.error(err);
            throw new Exception(err);
        }
        
        sendReq(stage, req, transParam, transactionId, responser, false, 10);
    }
    
    public void sendReq(IStage stage, IMsg req,
        long transParam, long transactionId,
        ServiceAddr responser, int timeout) throws Exception
    {
        sendReq(stage, req, transParam, transactionId,
            responser, false, timeout);
    }
    
    public void sendReq(IStage stage, IMsg req,
        long transParam, long transactionId,
        ServiceAddr responser, boolean returnReq) throws Exception
    {
        sendReq(stage, req, transParam, transactionId,
            responser, returnReq, 10);
    }
    
    public void sendReq(IStage stage, IMsg req,
        long transParam, long transactionId,
        ServiceAddr responser, boolean returnReq, int timeout) throws Exception
    {
        if (req == null)
        {
            String err="req is null";
            logger.error(err);
            throw new Exception(err);
        }
        
        long callId = m_call_id.addAndGet(1);
        int targetRpcStage = (int)(callId%m_rpc_stage_list.size());
        
        logger.debug("transaction_id:" + transactionId
            + ", trans_param:" + transParam
            + ", msg_type:" + req.getMsgType()
            + ", call_id:" + callId
            + ", target_rpc_stage:" + targetRpcStage
            + ", biz_stage_id:" + stage.getId()); 
            
        SendRpcReqEvent e = new SendRpcReqEvent();
        RpcHead rh = new RpcHead();
        CallOutRecordItem item = new CallOutRecordItem();
        
        e.msg = req;
        e.rpcHead = rh;
        e.callOutRecordItem = item;

        rh.msgType.set(req.getMsgType());
        rh.invoker.serviceType.set(0);
        rh.responser.set(responser);
        
        if (m_client_instance_id == 0)
        {
            ServiceStageMap ssm = ServiceStageMap.getInstance();
            m_client_instance_id = ssm.getSelfInstanceId(0);
        }
    
        rh.invoker.instanceId.set(m_client_instance_id);   
        rh.transactionId.set(transactionId);
        rh.transParam.set(transParam);
        rh.bizHandlerId.set(stage.getId());
        rh.callId.set(callId);
        
        item.rpcHead = rh;
        item.timeout = timeout;

        m_rpc_stage_list.get(targetRpcStage).getSink().enqueue(e);
    }
    
    private long m_client_instance_id = 0;
    public IMsg call(IMsg req, ServiceAddr responser, int timeout) throws Exception
    {
        if (req == null)
        {
            String err="req is null";
            logger.error(err);
            throw new Exception(err);
        }
        
        long callId = m_call_id.addAndGet(1);
        int targetRpcStage = (int)(callId%m_rpc_stage_list.size());
        
        logger.debug("msg_type:" + req.getMsgType()
            + ", call_id:" + callId
            + ", target_rpc_stage:" + targetRpcStage); 
            
        SendRpcReqEvent e = new SendRpcReqEvent();
        RpcHead rh = new RpcHead();
        CallOutRecordItem item = new CallOutRecordItem();
        
        e.msg = req;
        e.rpcHead = rh;
        e.callOutRecordItem = item;

        rh.msgType.set(req.getMsgType());
        rh.invoker.serviceType.set(0);
        rh.responser.set(responser);
        
        if (m_client_instance_id == 0)
        {
            ServiceStageMap ssm = ServiceStageMap.getInstance();
            m_client_instance_id = ssm.getSelfInstanceId(0);
        }
    
        rh.invoker.instanceId.set(m_client_instance_id); 
        rh.bizHandlerId.set(-1);  
        //rh.transactionId.set(transactionId);
        //rh.transParam.set(transParam);
        
        //e.bizHandlerId = stage.getId();
        //e.timeout = timeout;
        rh.callId.set(callId);
        
        item.rpcHead = rh;
        item.timeout = timeout;
        item.sync = true;

        m_rpc_stage_list.get(targetRpcStage).getSink().enqueue(e);
        
        logger.debug("Begin to wait for res");
        item.wait_res();
        if (item.resCode != 0)
        {
            throw new Exception("resCode:" + item.resCode);
        }
        return item.res;
    }        
            
    public void sendRes(RpcHead reqRpcHead, int resCode, IMsg res) throws Exception
    {
        int targetRpcStage = (int) (reqRpcHead.callId.get() % m_rpc_stage_list.size()); 
            
        logger.debug("call_id:" + reqRpcHead.callId
            + ", res_code:" + resCode
            + ", msg_type:" + (res != null ? res.getMsgType(): 0)
            + ", target_rpc_stage:" + targetRpcStage);
               
        SendRpcResEvent e = new SendRpcResEvent();
        e.reqRpcHead = reqRpcHead;
        e.resCode = resCode;
        e.msg = res;
        
    
        m_rpc_stage_list.get(targetRpcStage).getSink().enqueue(e);        
    }     
}
