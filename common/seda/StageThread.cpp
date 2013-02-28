#include <iostream>
#include "log/Logger.h"
#include "StageThread.h"


using namespace zonda::common;

StageThread::StageThread(IStage* self_stage)
    :m_event_handler(NULL), 
    m_event_source(NULL), 
    m_stop(false),
    m_trace(false)
{
    m_stage_name = self_stage->get_name();
}

void StageThread::set_event_handler(IEventHandler* handler)
{
    m_event_handler = handler;
}

void StageThread::set_event_source(IEventSource* event_source)
{
    m_event_source = event_source;
}
  
void StageThread::run()
{
    while (1)
    {
        const IEvent* e = NULL;
        if (m_event_source)
        {
            if (m_trace)
            {
                LOG_DEBUG("stage_name:" << m_stage_name << ", before time_dequeue");
            }        
            e = m_event_source->time_dequeue(1000);
            if (m_trace)
            {
                LOG_DEBUG("stage_name:" << m_stage_name << ", after time_dequeue");
            }  
        }
        else
        {
            LOG_FATAL("m_event_source is NULL, stage thread exit!");
            return;
        }
        //const IEvent* e = m_event_source->top();
        if (e == NULL)
        {
        	if (m_trace)
        	{
            	LOG_DEBUG("stage_name:" << m_stage_name 
            		<< ",e=" << e
            		<< ",m_stop=" << m_stop);
            }        		
            if (m_stop) break;
            else continue;
            
        }
        
        if (m_event_handler)
        {
            if (m_trace)
            {
                LOG_DEBUG("event_type:" << e->get_type());
            }
            m_event_handler->handle_event(e);
        }
        else
        {
            LOG_FATAL("m_event_source is NULL, stage thread exit");
            return;
        }      
    }
}

void StageThread::stop()
{
    m_stop = true;
    //std::cout << "StageThread::stop(), m_stop=" << m_stop << std::endl;
}

void StageThread::enable_trace()
{
   m_trace = true; 
}

void StageThread::disable_trace()
{
    m_trace = false;
}
    


