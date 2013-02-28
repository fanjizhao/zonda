#include <sys/time.h>
#include <errno.h>
#include <iostream>
#include <cstring>
#include <sys/select.h>
#include "TimerThread.h"
#include "TimerEvent.h"
#include "log/Logger.h"

namespace zonda
{
namespace common
{

TimerThread::TimerItem::TimerItem():
	id(0), interval(0),param(0), sink(NULL)
{
	expire_time.tv_sec = 0;
	expire_time.tv_usec = 0;
}

TimerThread::TimerThread():
	m_timer_seq(0), 
	m_max_timer_count(1000000),
	m_cond(m_lock),
	m_default_sink(NULL),
	m_stop(false),
	m_wait_item(NULL)
{

}

inline bool TimerThread::check_timer_canceled(TimerItem* &item)
{
    std::tr1::unordered_set<int>::iterator it;
    it = m_canceled_timers.find(item->id);
    if (it != m_canceled_timers.end())
    {
        LOG_DEBUG("timer_id:" << item->id 
            << " has been canceled, so discard it");
        m_canceled_timers.erase(it);
        delete item;
        item = NULL;
        return true;
    }
    
    return false;
}

void TimerThread::run()
{
    struct timeval now;
    m_lock.lock();
	while(!m_stop)
	{
	    while(m_queue.empty())
	    {
            m_cond.wait();
	    }
	    
		m_wait_item = m_queue.top();
        m_queue.pop();
	    if (check_timer_canceled(m_wait_item) == true)
	    {
	        continue;
	    }

		//NOTE: It seems always has time error, so we maually subtract
		//1 millsecond to make the timer accurate
		timeval n = m_wait_item->expire_time;
		if (n.tv_usec < 1000)
		{
		    n.tv_sec -= 1;
		    n.tv_usec += (1000000-1000);
		}
		else
		{
		    n.tv_usec -= 1000;
		}

	    //LOG_DEBUG("To wait on timer:" << m_wait_item->id); 
        int r = m_cond.time_wait(n);

        //if m_wait_item is NULL, that indicates this timer has been canceled        
        if (m_wait_item == NULL)
        {
            continue;
        }
        
	    if (check_timer_canceled(m_wait_item) == true)
	    {
	        continue;
	    }
	           	        
        if (r == 0)
        {
            //In this process, there is only one case that this cond will be signaled
            //An earilier timer has been put into the queue, so
            //we need to put the orginal item into the queue and
            //to refetch the top item from the queue
            m_queue.push(m_wait_item);
            m_wait_item = NULL;
            continue;
        }
        else if (r == ETIMEDOUT)
        {
            //We waited the specified time, in this case 
            //we need to handle the item we hold
            //As soon as the function return, it always get the lock.!!!
            //So we don't need to lock it again!!!!
            //LOG_DEBUG( " r = " << r);
        }
        else
        {
            //some error occured, we need to put
            //the item back into the queue, 
            //refetch it and wait again.
            //However, in practical, this case should
            //not occur.
            LOG_ERROR("Should not go here, cond_wait should not fail."
                << "Error:" << strerror(r));	 
            m_lock.lock();
            m_queue.push(m_wait_item);
            continue;
     
        }
		
		gettimeofday(&now, NULL);
		/*
		LOG_DEBUG("Expired Timer_id: " << m_wait_item->id 
			<< ", interval:" << m_wait_item->interval
			<< ", param: " << m_wait_item->param 
			<< ", expire_time.tv_sec: " << m_wait_item->expire_time.tv_sec
			<< ", expire_time.tv_usec: " << m_wait_item->expire_time.tv_usec
			<< ", diff: " 
			<< (now.tv_sec - m_wait_item->expire_time.tv_sec)*1000000 + now.tv_usec - m_wait_item->expire_time.tv_usec);
        */
	    
		TimerEvent* e = new TimerEvent(m_wait_item->id, m_wait_item->param);
		IEventSink* event_sink = m_wait_item->sink;
		
		        
		//Non-repeated item
		if (m_wait_item->interval <= 0)
		{
			delete m_wait_item;
			m_wait_item = NULL;
		}
		else
		{		
            //Repeated item
    		int usec = m_wait_item->expire_time.tv_usec + m_wait_item->interval*1000;
    		int sec = usec /1000000;
    		m_wait_item->expire_time.tv_sec += sec;
    		m_wait_item->expire_time.tv_usec = usec - sec*1000000;
    		m_queue.push(m_wait_item);
    		m_wait_item = NULL;
        }        
        
		m_lock.unlock();
		event_sink->enqueue(e);
		m_lock.lock();

	}//end while
	
    m_lock.unlock();
}

int TimerThread::init(size_t max_timer_count)
{
	m_lock.lock();
	m_max_timer_count = max_timer_count;
	m_lock.unlock();
	
	return 0;
}


void TimerThread::set_default_handler_sink(IEventSink* handler_sink)
{
	m_lock.lock();
	m_default_sink = handler_sink;
	m_lock.unlock();
}


void TimerThread::cancel_timer(int timer_id)
{
	LOG_DEBUG("cancel timer. timer_id:" << timer_id);
	m_lock.lock();
	
	if (m_wait_item && m_wait_item->id == timer_id)
	{
	    delete m_wait_item;
	    m_wait_item = NULL;
	    m_cond.signal();
	    m_lock.unlock();
	    LOG_DEBUG("canceled current timer. timer_id:" << timer_id);
	    return;
	}

	if (m_queue.empty())
	{
	    m_lock.unlock();
	    LOG_WARN("canceled timer_id:" << timer_id << " doesn't exist");
	    return;
	}
		
	TimerItem * item = m_queue.top();
	if (item->id == timer_id)
	{
	    m_queue.pop();
	    delete item;
	    m_lock.unlock();
	    LOG_DEBUG("canceled top timer_id:" << timer_id);
	    return;	    
	}
	m_canceled_timers.insert(timer_id);
	m_lock.unlock();
	LOG_DEBUG("canceled timer insert. timer_id:" << timer_id);
}

void TimerThread::cancel_all_timer()
{
	m_lock.lock();
	while (!m_queue.empty())
	{
	    TimerItem* item = m_queue.top();
	    delete item;
		m_queue.pop();
	}
	
	//Remove the current timer 
	delete m_wait_item;
	m_wait_item = NULL;
	m_cond.signal();
	
	
	m_lock.unlock();
}

int TimerThread::schedule(int64_t param, int delay, int interval)
{
	return schedule(m_default_sink, param, delay, interval);
}

int TimerThread::schedule(IEventSink* handler_sink, int64_t param, int delay, int interval)
{
    if (handler_sink == NULL)
    {
        LOG_WARN("schedule a timer param:" << param
            << " with NULL as handler_sink, denied!");
        return -1;
    }
    
    LOG_DEBUG("Begin schedule timer, param: " << param);
	m_lock.lock();
	if (m_queue.size() >= m_max_timer_count)
	{
	    m_lock.unlock();
	    LOG_WARN("Failed to schedule a new timer, too many timers. Limit:" 
	        << m_max_timer_count);
	    return -1;
	}

  	struct timeval now;	
	int r = gettimeofday(&now, NULL);
	if (r != 0)
	{
	    LOG_WARN("Failed to call gettimeofday. Error:" << strerror(errno));
	    return -1;
	}	
		
	TimerItem* item = new TimerItem();
	++m_timer_seq;
	if (m_timer_seq < 0) m_timer_seq =1;
	item->id = m_timer_seq;
	item->interval = interval;
	item->param = param;
	item->sink = handler_sink;

	time_t delta_sec = delay/1000; 
	suseconds_t usec = now.tv_usec + (delay-delta_sec*1000)*1000;
	if (usec >= 1000000)
	{
	    delta_sec += 1;
	    usec -= 1000000;
	}
	
		
	item->expire_time.tv_sec = now.tv_sec + delta_sec; 
	item->expire_time.tv_usec = usec;
	//LOG_DEBUG("BBBBBBBBBBBBBB");
	m_queue.push(item);
	r = item->id;
	//LOG_DEBUG("~BBBBBBBBBBBBB");

	
	LOG_DEBUG("TimerThread, push an item," 
	    << ", id:" << item->id
		<< ", param:" << item->param
		<< ", delay:" << delay
		<< ", interval:" << item->interval
		<< ", now.tv_sec: " << now.tv_sec
		<< ", now.tv_usec:" << now.tv_usec
		<< ", expire_time.tv_sec: " << item->expire_time.tv_sec
		<< ", expire_time.tv_usec: " << item->expire_time.tv_usec);
    
    if ((m_wait_item == NULL && (item == m_queue.top() )  )
        || (m_wait_item && (timeval_compare(item->expire_time, m_wait_item->expire_time) < 0 )) )
	{
	    //Only if this new one becamed the new top,
	    //we need to signal.
	    LOG_DEBUG("Need to break up the thread block on time_cond"); 
	    m_cond.signal();
	}			
	m_lock.unlock();
    LOG_DEBUG("End schedule timer, param: " << param);
	return r;
}

void TimerThread::stop()
{
	m_lock.lock();
	m_stop = true;
	m_cond.signal();
	m_lock.unlock();
}

} //namespace common
} //namespace zonda

