#include <iostream>
#include <cstring>
#include "EventQueue.h"
#include "thread/Thread.h"
#include "log/Logger.h"

using namespace zonda::common;

EventQueue::EventQueue():
    m_queue(NULL),
    m_produce_cond(m_lock), 
    m_consume_cond(m_lock), 
    m_queue_capacity(20000),
    m_queue_size(0),
    m_head(-1),
    m_tail(-1)
{
    
}    

int EventQueue::set_capacity(size_t capacity)
{
    m_lock.lock();
    if (m_queue != NULL)
    {
        m_lock.unlock();
        return -1;
    }
    
    m_queue_capacity = capacity;
    m_queue = new const IEvent* [m_queue_capacity];
    
    m_lock.unlock();
    
    return 0;
}

      
void EventQueue::enqueue(const IEvent* event)
{
	
	while (1)
	{
		m_lock.lock();
		if (m_queue_size == 0)
		{
            push(event);			
			m_consume_cond.broadcast();
			break;
		}
		else if (m_queue_size < m_queue_capacity)
		{
            push(event);
			break;
		}
		else //size >= m_queue_max_len
		{
		    LOG_INFO("Queue is full, increase the size!!");
			m_produce_cond.wait();
        	if (m_queue_size == 0)
        	{
        		push(event);
				m_consume_cond.broadcast();
				break;		
        	}
        	else if (m_queue_size < m_queue_capacity)
        	{
        		push(event);
        		break;
        	}
        	else //size >= m_queue_capacity
        	{
        		m_lock.unlock();
        	}
		}
	}

	m_lock.unlock();
	

}

size_t EventQueue::size()
{
    return m_queue_size;
}

const IEvent* EventQueue::dequeue()
{
    const IEvent* e = NULL;
    m_lock.lock();
    
    while(1)
    {
        if (m_queue_size == 0)
        {
            m_consume_cond.wait();
            continue;
        }
	    if (m_queue_size >= m_queue_capacity)
	    {
	        m_produce_cond.broadcast();
	    }        
        e = front();
    }
    m_lock.unlock();    
    return e;
}

const IEvent* EventQueue::time_dequeue(int millseconds)
{
    const IEvent* e = NULL;
    m_lock.lock();
    int r = 0;
    if ( m_queue_size == 0)
    {
        //LOG_DEBUG("time_dequeue, before wait, Thread id:" 
        //	<< Thread::self_id() << ",size:" << m_queue.size());
        r = m_consume_cond.time_wait(millseconds);
        //LOG_DEBUG("time_dequeue, after wait, " << strerror(r) << ",Thread id:" 
        //	<< Thread::self_id() << ",size:" << m_queue.size());

	    //Not timeout, we need to get the queue size again
	    //because before this thread is scheduled, other threads
	    //might be scheduled and fetched the events produced by the
	    //thread which sent broadcast to this thread
    }
    
	if (r == 0 &&  m_queue_size > 0)
	{
	    if (m_queue_size >= m_queue_capacity)
	    {
	        m_produce_cond.broadcast();
	    }
        e = front();
	}    
    m_lock.unlock();
    return e;
    
}

size_t EventQueue::time_dequeue(int millseconds, 
        const IEvent* event_array[], const size_t array_size)

{
    const IEvent* e = NULL;
    m_lock.lock();
    int r = 0;
    size_t real_num = 0;
    if ( m_queue_size == 0)
    {
        //LOG_DEBUG("time_dequeue, before wait, Thread id:" 
        //	<< Thread::self_id() << ",size:" << m_queue.size());
        r = m_consume_cond.time_wait(millseconds);
        //LOG_DEBUG("time_dequeue, after wait, " << strerror(r) << ",Thread id:" 
        //	<< Thread::self_id() << ",size:" << m_queue.size());

	    //Not timeout, we need to get the queue size again
	    //because before this thread is scheduled, other threads
	    //might be scheduled and fetched the events produced by the
	    //thread which sent broadcast to this thread
    }
    
	if (r == 0 &&  m_queue_size > 0)
	{
	    if (m_queue_size >= m_queue_capacity)
	    {
	        m_produce_cond.broadcast();
	    }
	    
	    real_num = m_queue_size > array_size ? array_size : m_queue_size;
	    for (size_t i=0; i<real_num; ++i)
	    {
             event_array[i] = front();
        }
	}    
    m_lock.unlock();
    
    return real_num;    
} 



