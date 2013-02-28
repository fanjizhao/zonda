package com.alibaba.platform.zonda.common.seda;

import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import com.alibaba.platform.zonda.logger.lib.AliLoggerFactory;


public class EventQueue implements IEventQueue
{
    private Logger logger = AliLoggerFactory.getLogger("SEDA", getClass());
    private LinkedBlockingQueue<IEvent> m_queue = new LinkedBlockingQueue<IEvent>();
    private int m_capacity;
    
    public EventQueue() {}
    public void enqueue(IEvent event)
    {
        while (true)
        {
            try
            {
                //logger.debug("before put event.getType()");
                m_queue.put(event);
                break;
            }
            catch (InterruptedException ie)
            {
                continue;
            }
            catch (NullPointerException ne)
            {
                logger.warn("event is null");
                return;
            }
        }
    }
    public int size()
    {
        return m_queue.size();
    }
    
    public void setCapacity(int capacity)
    {
        m_capacity = capacity;
    }
    
    public IEvent dequeue()
    {
        while (true)
        {
            try
            {
                return m_queue.take();
            }
            catch (InterruptedException ie)
            {
                continue;
            }
        }
    }
    
    public IEvent timeDequeue(int millseconds)
    {
        try
        {
            return m_queue.poll(millseconds, TimeUnit.MILLISECONDS);
        }
        catch (InterruptedException ie)
        {
            return null;
        }
    }
    public int getCapacity()
    {
        return m_capacity;
    }  
    
}
