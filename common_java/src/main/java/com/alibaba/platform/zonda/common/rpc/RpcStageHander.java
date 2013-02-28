package com.alibaba.platform.zonda.common.rpc;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import com.alibaba.platform.zonda.common.seda.*;
import com.alibaba.platform.zonda.common.util.TimerEvent;
import com.alibaba.platform.zonda.common.util.GlobalTimer;
import java.util.HashMap;
import java.util.ArrayList;
import java.nio.ByteBuffer;


import com.alibaba.platform.zonda.common.config.Configer;
import com.alibaba.platform.zonda.common.config.event.*;
import com.alibaba.platform.zonda.common.config.CreateMode;
import com.alibaba.platform.zonda.common.config.MetaInfo;
import com.alibaba.platform.zonda.common.config.exception.ConfigUnintializedException;
import com.alibaba.platform.zonda.common.config.exception.CreateNodeFailedException;
import com.alibaba.platform.zonda.common.config.exception.SetValueFailedException;
import com.alibaba.platform.zonda.common.config.zk.ZkConfiger;
import com.alibaba.platform.zonda.logger.lib.AliLoggerFactory;


class RpcStageHandlerParam
{
    RpcServiceParam rpcParam;
    CallOutRecord   callOutRecord;
}


public class RpcStageHander implements IEventHandler
{
    private Logger logger = AliLoggerFactory.getLogger(Constants.MODULE_NAME, getClass());
    private IMsgFactory m_msg_factory;
    private CallOutRecord   m_call_out_record;
    private int   m_call_in_count;
    private IEventSink m_self_sink;
    private int m_stage_id;
    private ZMQSender   m_trans_sender = new ZMQSender();
    private long m_client_instance_id;
    private final int TIMEOUT_TIMER = 1;
    //10 seconds
    private final int TIMEOUT_CHECK_INTERVAL = 10000;
    public RpcStageHander()
    {
        m_client_instance_id = 0;
        m_call_in_count = 0;
    }
    
    public void init(IStage stage, Object param)
    {
        RpcStageHandlerParam stage_param = (RpcStageHandlerParam)param;
    
        RpcServiceParam p = (RpcServiceParam)(stage_param.rpcParam);
        m_msg_factory = p.msgFactory;
        m_call_out_record = stage_param.callOutRecord;
        //m_timer = stage_param->timer;
    
        m_self_sink = stage.getSink();
        m_stage_id = stage.getId();
    
        m_trans_sender.init(null);
        
        GlobalTimer.getInstance().schedule(
            m_self_sink, 
            TIMEOUT_TIMER,
            1000,
            TIMEOUT_CHECK_INTERVAL);    
    }
    public void handleEvent(IEvent event)
    {
        if (event == null)
        {
            //logger.warn("event is null");
            return;
        }
        int eventType = event.getType();
        switch (eventType)
        {
            case EventType.SEND_RPC_REQ_EVENT:
                handleSendRpcReqEvent((SendRpcReqEvent)event);
                return;
            case EventType.SEND_RPC_RES_EVENT:
                handleSendRpcResEvent((SendRpcResEvent)event);
                return;
            case EventType.CONFIG_CHANGE_EVENT:
                handleConfChange((ConfigChangeEvent)event);
                return;
            case EventType.TIMER_EVENT:
                handleTimerEvent((TimerEvent)event);
                return;
            default:
                //logger.error("Unknow event type:" + eventType);
                return;
        }         
    }


    private void handleSendRpcReqEvent(SendRpcReqEvent e)
    {
        int msgType = e.msg.getMsgType();
        //logger.debug("msg_type:" + msgType
        //    + ",transactionId:" + e.transactionId
        //    + ",transParam: " + e.transParam
        //    + ",callId:" + e.callId);
        
        try
        {
            RealRpcAddr realAddr;
            realAddr = RouteTable.getInstance().getRealAddr(e.rpcHead.responser);

            TransHead transHead = new TransHead();
            transHead.version.set(1);
            transHead.rpcHeadLen.set(e.rpcHead.size());
            transHead.callId.set(e.rpcHead.callId.get());

            byte[] buffMsg = m_msg_factory.serialize_body(e.msg);
            transHead.rpcMsgLen.set(buffMsg.length);
            ByteBuffer wholeData = ByteBuffer.allocate(buffMsg.length 
                + transHead.size() + e.rpcHead.size());
            
            wholeData.put(transHead.getByteBuffer());
            wholeData.put(e.rpcHead.getByteBuffer());
            wholeData.put(buffMsg);

            logger.debug("Begin to send a msg, RpcHead, msg_type:" + e.rpcHead.msgType
                + ", call_id:" + e.rpcHead.callId
                +  ", transaction_id:" + e.rpcHead.transactionId
                + ", responser_type:" + e.rpcHead.responser.serviceType
                + ", invoker_type:" + e.rpcHead.invoker.serviceType
                + ", biz_handler_id:" + e.rpcHead.bizHandlerId
                + ", returnReq:" + (e.callOutRecordItem.req == null? "false" : "true"));            

            m_call_out_record.add(e.callOutRecordItem);

            m_trans_sender.sendData(e.rpcHead.callId.get(), wholeData.array(),
                realAddr.ip, realAddr.port); 

        }
        catch(Exception ex)
        {
            try
            {
                m_call_out_record.remove(e.rpcHead.callId.get());
            }
            catch(Exception myEx)
            {
                //It should remove the element successfully.
                //So we don't need handle any exception
            }
            //logger.error("Failed to handle send req. Err:" + ex.getMessage());
            ex.printStackTrace();
            
            //For sync call, to notify the call thread
            if (e.callOutRecordItem.sync == true)
            {
                e.callOutRecordItem.resCode = (short)RpcHead.RpcResCode.SEND_FAIL.ordinal();
                e.callOutRecordItem.notify_res();
                return;
            }
            
            //For async call, create an error res, throw it back to the biz stage
            e.rpcHead.msgType.set(0 - e.rpcHead.msgType.get());
            e.rpcHead.resCode.set((short)RpcHead.RpcResCode.SEND_FAIL.ordinal());
    
            RpcEvent rpcEvent = new RpcEvent(e.rpcHead);
            rpcEvent.setReq(e.callOutRecordItem.req);
            IEventSink sink = null;
            sink = StageMgr.getInstance().getStage(e.rpcHead.bizHandlerId.get()).getSink();
    
            sink.enqueue(rpcEvent);
        }
          
    }
    private void handleSendRpcResEvent(SendRpcResEvent e)
    {
        //logger.debug("call_id:" + e.reqRpcHead.callId
        //    + ", res_code:" + e.resCode
        //    + ", msg_type:" + (e.msg != null ? e.msg.getMsgType() : 0));
    
    
        RpcHead reqRpcHead = e.reqRpcHead;
    
        ServiceAddr reqInvokerAddr;
        reqInvokerAddr = reqRpcHead.invoker;        
        
        RealRpcAddr realAddr;
        try
        {
            realAddr = RouteTable.getInstance().getRealAddr(reqInvokerAddr);
            TransHead transHead = new TransHead();
            
            
            
            RpcHead rpcHead = new RpcHead();
            rpcHead.msgType.set(0-reqRpcHead.msgType.get());
            rpcHead.responser.set(reqRpcHead.invoker);
            rpcHead.invoker.set(reqRpcHead.responser);
            rpcHead.transactionId.set(reqRpcHead.transactionId.get());
            rpcHead.transParam.set(reqRpcHead.transParam.get());
            rpcHead.callId.set(reqRpcHead.callId.get());
            rpcHead.bizHandlerId.set(reqRpcHead.bizHandlerId.get());
            rpcHead.resCode.set((short)0);
            
            transHead.version.set(1);
            transHead.rpcHeadLen.set(rpcHead.size());
            
            byte[] buffMsg = m_msg_factory.serialize_body(e.msg);
            transHead.rpcMsgLen.set(buffMsg.length);
            transHead.callId.set(e.reqRpcHead.callId.get());
            
            ByteBuffer wholeData = ByteBuffer.allocate(buffMsg.length 
                + transHead.size() + rpcHead.size());
            
            wholeData.put(transHead.getByteBuffer());
            wholeData.put(rpcHead.getByteBuffer());
            wholeData.put(buffMsg);
                        
            //logger.debug("Begin to send an res, TransHead, version:" + transHead.version
            //    + ", rpcHeadLen:" + transHead.rpcHeadLen
            //    + ", rpcMsgLen:" + transHead.rpcMsgLen
            //    + ", msg size:" 
            //    + (transHead.rpcHeadLen.get() + transHead.rpcMsgLen.get() + transHead.size()));

            m_trans_sender.sendData(e.reqRpcHead.callId.get(), wholeData.array(),
                realAddr.ip, realAddr.port);             
        }
        catch (Exception ex)
        {
            //logger.error("Failed to handle SendRpcResEvent. Err:"
            //    + ex.getMessage());
            ex.printStackTrace();
        }
    }
    
    private void handleTimerEvent(TimerEvent e)
    {
        //logger.debug("param:" + e.getParam() + ", id:" + e.getId());
        m_call_out_record.handleTimeoutItems();
    }
    
    private void handleConfChange(ConfigChangeEvent e)
    {
        //logger.info("Received conf change event, changeType:" 
        //    + e.getChangeType());
        try
        {
            if (e.getPath().compareTo(Configer.ZONDA_ROUTE_TABLE_PATH) == 0)
            {
                //logger.info("route_table changed, new size:" + e.getData().length);
                RouteTable.getInstance().load(new String(e.getData()));
            }
            else if (e.getPath().compareTo(Configer.ZONDA_ROUTE_STRATEGY_PATH) == 0)
            {
                //logger.info("route strategy changed, new size:" + e.getData().length);
                RouteStrategy.getInstance().load(new String(e.getData()));
            }
            else
            {
                //logger.warn("Did not watch the data of " + e.getPath());
            }           
        }
        catch( Exception ex)
        {
            //logger.error("Failed to update the conf. "
            //    + ", path:" + e.getPath()
            //    + ", Err:" + ex.getMessage());
            ex.printStackTrace();
        }

    }
    
    //! Removed successfully, return 0, not found record, return -1
    private void removeCallOutRecord(long call_id)
    {
    }


}  
