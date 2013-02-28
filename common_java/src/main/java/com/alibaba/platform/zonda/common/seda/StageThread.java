package com.alibaba.platform.zonda.common.seda;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import com.alibaba.platform.zonda.logger.lib.AliLoggerFactory;

class StageThread implements Runnable
{
    private Logger logger = AliLoggerFactory.getLogger("SEDA", getClass());
    private IEventHandler m_event_handler = null;
    private IEventSource m_event_source = null;
    private boolean m_stop = false;
    private boolean m_trace = false;
    private String m_stage_name = null;  
        
    public StageThread(IStage stage)
    {
        m_stage_name = stage.getName();
    }
    public void setEventHandler(IEventHandler handler)
    {
        m_event_handler = handler;
    }
    public void setEventSource(IEventSource eventSource)
    {
        m_event_source = eventSource;
    }
    
    public void stop()
    {
        m_stop = true;
    }
    public void enable_trace()
    {
        m_trace = true;
    }
    public void disable_trace()
    {
        m_trace = false;
    }

    public void run()
    {
        while (true)
        {
            IEvent e = null;
            if (m_event_source != null)
            {
                if (m_trace)
                {
                    logger.debug("stage_name:" + m_stage_name + ", before time_dequeue");
                }        
                e = m_event_source.timeDequeue(1000);
                if (m_trace)
                {
                    logger.debug("stage_name:" + m_stage_name + ", after time_dequeue");
                }  
            }
            else
            {
                logger.error("m_event_source is null, stage thread exit!");
                return;
            }

            if (e == null)
            {
            	if (m_trace)
            	{
                	logger.debug("stage_name:" + m_stage_name 
                		+ ",m_stop=" + m_stop);
                }        		
                if (m_stop) break;
                else continue;
                
            }
            
            if (m_event_handler != null)
            {
                if (m_trace)
                {
                    logger.debug("event_type:" + e.getType());
                }
                m_event_handler.handleEvent(e);
            }
            else
            {
                logger.error("m_event_source is null, stage thread exit");
                return;
            }      
        }        
    }


  
}
