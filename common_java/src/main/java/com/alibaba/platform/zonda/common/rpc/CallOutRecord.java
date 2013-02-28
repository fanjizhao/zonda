package com.alibaba.platform.zonda.common.rpc;

import java.util.HashMap;
import java.util.Map;
import java.util.Iterator;
import com.alibaba.platform.zonda.logger.lib.AliLoggerFactory;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import com.alibaba.platform.zonda.common.seda.*;

class CallOutRecordItem
{
    public RpcHead rpcHead;
    public long sendTime = System.currentTimeMillis()/1000L;
    //The unit is second
    public int timeout;
    public IMsg req = null;
    public IMsg res = null;
    public short resCode = 0;
    public boolean sync = false;
    public boolean timeoutFlag = false;
    public synchronized void wait_res() throws Exception
    {
        if (sync == true && res == null )
        {
            wait(timeout*1000);
            if (res == null)
            {
                //The res has not come back yet, namely timeout
                timeoutFlag = true;
                resCode = (short)RpcHead.RpcResCode.TIMEOUT.ordinal();
            }
        }
    }
    
    public synchronized void notify_res()
    {
        notify();
    }    
}

class CallOutRecord
{
    private Logger logger = AliLoggerFactory.getLogger(Constants.MODULE_NAME, getClass());
    private HashMap<Long, CallOutRecordItem> m_map = new HashMap<Long, CallOutRecordItem>(); 
    public synchronized void add(CallOutRecordItem item)
    {        
        m_map.put(item.rpcHead.callId.get(), item);
    }
    
    public synchronized CallOutRecordItem remove(long callId) throws Exception
    {
        CallOutRecordItem item = m_map.remove(callId);
        if (item == null)
        {
            throw new Exception("Not Found such record");
        }
        
        return item;
    }
    
    public synchronized int handleTimeoutItems()
    {
        Iterator it = m_map.entrySet().iterator();
        Long now = System.currentTimeMillis()/1000L;
        while (it.hasNext()) 
        {
            Map.Entry entry = (Map.Entry) it.next();
            Long callId = (Long)entry.getKey();
            CallOutRecordItem item = (CallOutRecordItem)entry.getValue();
            
            if (now - item.sendTime < 30)
            {
                continue;
            }
            
            
            if (item.sync == true)
            {
                if (item.timeoutFlag == true)
                {
                    //The call thread has returen the timeout
                    //Here only need to remove the item

                }
                else
                {
                    item.resCode = (short)RpcHead.RpcResCode.SEND_FAIL.ordinal();
                    item.notify_res();                
                }
                
                it.remove();
                continue;
            }
            
            
            //For the async call, need to make a fake res for the real invoker stage
            RpcHead rpcHead = item.rpcHead;
            rpcHead.msgType.set(0 - rpcHead.msgType.get());
            rpcHead.resCode.set((short)RpcHead.RpcResCode.TIMEOUT.ordinal());
            logger.info("call_id:" + rpcHead.callId.get()
                + " times out. biz_handler_id:"
                + rpcHead.bizHandlerId.get());

            RpcEvent rpcEvent = new RpcEvent(rpcHead);
            rpcEvent.setReq(item.req);
            it.remove();

            IEventSink sink = null;
            sink = StageMgr.getInstance().getStage(rpcHead.bizHandlerId.get()).getSink();

            sink.enqueue(rpcEvent);            
        } 
        
        return 0;
    }
    
    public synchronized int size()
    {
        return m_map.size();
    }
    

    
    public static void main(String[] args)
    {
        CallOutRecord callOutRecord = new CallOutRecord();
        CallOutRecordItem item1 = new CallOutRecordItem();
        CallOutRecordItem item2 = new CallOutRecordItem();
        
        item1.rpcHead = new RpcHead();
        item1.rpcHead.callId.set(1334039910544001L);
        callOutRecord.add(item1);
        item2.rpcHead = new RpcHead();
        item2.rpcHead.callId.set(1334039910544002L);
        callOutRecord.add(item2);
        
        try
        {
            callOutRecord.remove(1334039910544001L);
            callOutRecord.remove(1334039910544002L);
        }
        catch(Exception e)
        {
            System.out.println(e.getMessage());
            e.printStackTrace();
        }
        
    }   
}