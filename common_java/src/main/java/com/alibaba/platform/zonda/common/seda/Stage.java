package com.alibaba.platform.zonda.common.seda;


import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import java.util.Vector;
import com.alibaba.platform.zonda.logger.lib.AliLoggerFactory;

public class Stage implements IStage
{
    private Logger logger = AliLoggerFactory.getLogger("SEDA", getClass());
    private String m_name = null;
    private boolean m_init = false;
    private EventQueue m_queue = new EventQueue();
    private int m_id = -1;
    private int m_queue_capacity = 10000;
    private int m_thread_count = 1;
    private StageThreadMode m_thread_mode = StageThreadMode.EXLUSIVE;
    private Vector<IEventHandler> m_handler_vector = new Vector<IEventHandler>();
    private Vector<Thread> m_thread_vector = new  Vector<Thread>();
    private Vector<StageThread> m_runnable_vector = new Vector<StageThread>();
    private String m_handler_class_name = null;
    
    
    public Stage(String eventHandlerClassName, String name)
    {
        m_name = name;
        m_handler_class_name = eventHandlerClassName;
        m_id = StageMgr.getInstance().regStage(this);
    }
    
    public Stage(String eventHandlerClassName, String name, int queueCapacity,  
            int threadCount, StageThreadMode threadMode)
    {
        m_name = name;
        m_queue_capacity = queueCapacity;
        m_thread_count = threadCount;
        m_thread_mode = threadMode;
        m_handler_class_name = eventHandlerClassName;
        m_id = StageMgr.getInstance().regStage(this);
    }
    public void init(Object handlerParam)
    {
        m_queue.setCapacity(m_queue_capacity);
        
        //Create the handlers
        Class c = null;
        try
        {
            c = Class.forName(m_handler_class_name);
        }
        catch (ClassNotFoundException e)
        {
            String err = "No found class " + m_handler_class_name;
            logger.error(err);
            throw new RuntimeException(err);
        }
        int handlerCount = 1;
        if (m_thread_mode != StageThreadMode.SHARED)
        {
            handlerCount = m_thread_count;
        }

        for (int i=0; i<handlerCount; ++i)
		{
            IEventHandler handler = null;
            try
            {
                handler = (IEventHandler)c.newInstance();
            }
            catch (InstantiationException e)
            {
                String err = "Failed to create instance for " + m_handler_class_name;
                logger.error(err);
                throw new RuntimeException(err);
            }
            catch (IllegalAccessException e)
            {
                String err = "Failed to create instance for " 
                    + m_handler_class_name 
                    + ", IllegalAccessException";
                logger.error(err);
                throw new RuntimeException(err);
            }
            handler.init(this, handlerParam);
            m_handler_vector.add(handler);		
		}
		
		//Create the thread
	    for (int i=0; i<m_thread_count; ++i)
    	{
    		StageThread r = new StageThread(this);
    		if (m_thread_mode == StageThreadMode.SHARED)
    		{
    			r.setEventHandler(m_handler_vector.get(0));
    		}
    		else
    		{
    			r.setEventHandler(m_handler_vector.get(i));
    		}
    		r.setEventSource(m_queue);
    		    		
    		Thread t = new Thread(r, m_name + "_" + i);    				
    		
            m_runnable_vector.add(r);
    		m_thread_vector.add(t);
    	}
    	
    	m_init = true;	
    }

    public String getName()
    {
        return m_name;
    }
    public IEventSink getSink()
    {
        return m_queue;
    }
    public int getId()
    {
        return m_id;
    }
    public void start()
    {
        if (m_init == false)
        {
            String err = "Please init the stage before to call start()";
            logger.error(err);
            throw new RuntimeException(err);
        }
        for (int i=0; i<m_thread_vector.size(); ++i)
        {
            Thread t = m_thread_vector.get(i);
            t.start();
        }
    }

    //! stop all the threads in this stage
    public void stop()
    {
        for (int i=0; i<m_runnable_vector.size(); ++i)
        {
            StageThread r = m_runnable_vector.get(i);
            r.stop();
        }        
    }
    //! Just for debug
    public void enableTrace()
    {
        for (int i=0; i<m_runnable_vector.size(); ++i)
        {
            StageThread r = m_runnable_vector.get(i);
            r.enable_trace();
        }  
    }
    public void disableTrace()
    {
        for (int i=0; i<m_runnable_vector.size(); ++i)
        {
            StageThread r = m_runnable_vector.get(i);
            r.disable_trace();
        }         
    }     
}
