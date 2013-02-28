#ifndef ZONDA_COMMON_STAGE_H_
#define ZONDA_COMMON_STAGE_H_

#include <iostream>
#include <vector>
#include "IStage.h"
#include "StageThread.h"
#include "StageMgr.h"
#include "log/Logger.h"
#include "EventDispatcher.h"

namespace zonda
{
namespace common
{
    
template <class TypeOfHandler, class TypeOfQueue = EventQueue>
class Stage: public IStage
{
public:
	Stage(const char *name, 
            size_t queue_capacity,  
            int thread_count = 1, 
            StageThreadMode thread_mode = EXLUSIVE);
              
    virtual int init(void *handler_param); 
    virtual void set_event_dispatcher(EventDispatcher* dispatcher);        	
    virtual const char* get_name();
    virtual IEventSink* get_sink();
    virtual uint16_t get_id();
    virtual int start();
    virtual void stop();
    virtual ~Stage();     
    virtual void enable_trace();
    virtual void disable_trace();
private:
	std::string m_name;
	TypeOfQueue* m_event_queues;
	EventDispatcher* m_event_dispatcher;
	size_t m_queue_capacity;
	std::vector<TypeOfHandler*> m_handler_vector;
	int m_thread_count;
	StageThreadMode m_thread_mode;
	std::vector<StageThread*> m_thread_vector;
	uint16_t m_my_id;
};

}//namespace common
}//namespace zonda


using namespace zonda::common;

template <class TypeOfHandler, class TypeOfQueue>
Stage<TypeOfHandler, TypeOfQueue>::Stage(const char *name, 
            size_t queue_capacity,  
            int thread_count , 
            StageThreadMode thread_mode):
	m_name(name),
	//m_event_queue(),
	m_queue_capacity(queue_capacity),
	m_thread_count(thread_count),
	m_thread_mode(thread_mode)
{
	m_event_queues = new TypeOfQueue[thread_count];
	m_event_dispatcher = new EventDispatcher();
	
	//Indicates this stage is not registered.
	m_my_id = -1;
}

template <class TypeOfHandler, class TypeOfQueue>
Stage<TypeOfHandler, TypeOfQueue>::~Stage()
{
	if (m_my_id != 0xFFFF)
	{
	    StageMgr::instance()->dereg_stage(this);
	}
	//NOTE:It seems to free allocated thread objects and 
	//event handler objects is uncessary since when we free the stage
	//object, that means we want to end the process.
	//On the other hand, if we want to free the thread objects and 
	//event handler objects, we must free thread objects first because
	//the event handler objects are used by these thread objects.
	//To free thread objects, we need to ensure the real OS thread is alreay
	// stoped. That is too troublesome.	
}


template <class TypeOfHandler, class TypeOfQueue>
int Stage<TypeOfHandler, TypeOfQueue>::init(void* handler_param)
{
   
    int r = 0;
    for (int i=0; i<m_thread_count; ++i)
    {
        r = m_event_queues[i].set_capacity(m_queue_capacity);
        
        if (r != 0)
        {
    	    LOG_FATAL("Failed to set_capacity of the queue");
    	    return r;        
        }
        
        //LOG_DEBUG("m_event_queues[" << i << "] size=" << (long)&m_event_queues[i]);
    }
    
   	//Create the handlers
	if (m_thread_mode == SHARED)
	{
		TypeOfHandler* handler = new TypeOfHandler();
		r = handler->init(this, handler_param);
		if (r != 0) 
		{
		    LOG_FATAL("Failed to init the handler");
		    return r;
		}
		m_handler_vector.push_back(handler);
	}
	else
	{		
		for (int i=0; i<m_thread_count; ++i)
		{
			TypeOfHandler* handler = new TypeOfHandler();
			int r = handler->init(this, handler_param);
			if (r)
			{
			    LOG_FATAL("Failed to init the handler");
				return -1;
			}
			
			m_handler_vector.push_back(handler);		
		}
	}
	
	//Create the thread
	for (int i=0; i<m_thread_count; ++i)
	{
		StageThread* t = new StageThread(this);
		if (m_thread_mode == SHARED)
		{
			t->set_event_handler(m_handler_vector[0]);
		}
		else
		{
			t->set_event_handler(m_handler_vector[i]);
		}
		
		char thread_name[200];
		sprintf(thread_name, "%s_%d", m_name.c_str(), i);
		t->set_thread_name(thread_name);
				
		
		t->set_event_source(&m_event_queues[i]);
		m_thread_vector.push_back(t);
		

	}
	
	IEventQueue* queues[m_thread_count];
	for (int i=0; i<m_thread_count; ++i)
	{
	    queues[i] = &m_event_queues[i];
	}
	m_event_dispatcher->set_internal_queues(queues, m_thread_count);
	
	r = StageMgr::instance()->reg_stage(this, m_my_id);
	if ( r!= 0)
	{
	    LOG_FATAL("Failed to reg_stage");
	    return r;
	}
	
	return 0;
}     

template <class TypeOfHandler, class TypeOfQueue>
void Stage<TypeOfHandler, TypeOfQueue>::set_event_dispatcher(EventDispatcher* dispatcher)
{
    delete m_event_dispatcher;
    m_event_dispatcher = dispatcher;
    if (m_event_queues != NULL)
    {
    	IEventQueue* queues[m_thread_count];
    	for (int i=0; i<m_thread_count; ++i)
    	{
    	    queues[i] = &m_event_queues[i];
    	}
    	m_event_dispatcher->set_internal_queues(queues, m_thread_count);
    }
}

template <class TypeOfHandler, class TypeOfQueue>
const char* Stage<TypeOfHandler, TypeOfQueue>::get_name()
{
	return m_name.c_str();
}

template <class TypeOfHandler, class TypeOfQueue>
IEventSink* Stage<TypeOfHandler, TypeOfQueue>::get_sink()
{
	//return &m_event_queue;
	return m_event_dispatcher;
}

template <class TypeOfHandler, class TypeOfQueue>
uint16_t Stage<TypeOfHandler, TypeOfQueue>::get_id()
{
	return m_my_id;
}

template <class TypeOfHandler, class TypeOfQueue>
int Stage<TypeOfHandler, TypeOfQueue>::start()
{
    LOG_DEBUG("stage_name:" << m_name << ", thread count:" << m_thread_vector.size()); 
	for (size_t i=0; i<m_thread_vector.size(); ++i)
	{
		int r = m_thread_vector[i]->start();
		if (r != 0)
		{
			return -1;
		}		
	}
	
	return 0;
}

template <class TypeOfHandler, class TypeOfQueue>
void Stage<TypeOfHandler, TypeOfQueue>::stop()
{
	for (size_t i=0; i<m_thread_vector.size(); ++i)
	{
		std::cout << "Stage::stop(), " 
			<< std::endl;
		m_thread_vector[i]->stop();
	}
	
}

template <class TypeOfHandler, class TypeOfQueue>
void Stage<TypeOfHandler, TypeOfQueue>::enable_trace()
{
	for (size_t i=0; i<m_thread_vector.size(); ++i)
	{
		m_thread_vector[i]->enable_trace();
	}    
}

template <class TypeOfHandler, class TypeOfQueue>
void Stage<TypeOfHandler, TypeOfQueue>::disable_trace()
{
	for (size_t i=0; i<m_thread_vector.size(); ++i)
	{
		m_thread_vector[i]->disable_trace();
	}    

}




#endif //ZONDA_COMMON_STAGE_H_
