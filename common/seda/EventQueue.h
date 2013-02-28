#ifndef ZONDA_COMMON_EVENTQUEUE_H_
#define ZONDA_COMMON_EVENTQUEUE_H_


#include <queue>
#include <list>
#include "IEventQueue.h"
#include "IEvent.h"
#include "thread/ThreadLock.h"
#include "thread/ThreadCond.h"

namespace zonda
{
namespace common
{
using namespace std;
   
class EventQueue: public IEventQueue
{
public:
    EventQueue();
    
    //! Put an event into the queue, if the queue is full, 
    //! this function will block until the queue is not full
    //! and put the event into the queue.
    virtual void enqueue(const IEvent* event);
    
    //! Returns the number of elements in the queue
    virtual size_t size();
    
    //0, success, -1 failed, must be called before the first calling enqueue
    virtual int set_capacity(size_t capacity);
    
    //! Get the top element and remove the element from the queue, 
    //! if the queue is empty, this method will block until it
    //! get an event 
    virtual const IEvent* dequeue();
    
    //! The method must return within the millseconds.
    //! In other worlds, it blocks for millseconds at most.
    //! If millseconds is 0, the method returns immediately
    //! no matter whether if the queue is empty(return NULL) 
    //! or not(return a real event).
    virtual const IEvent* time_dequeue(int millseconds);
    
    virtual size_t time_dequeue(int millseconds, 
        const IEvent* event_array[], const size_t array_size);    

private:
    void push(const IEvent* e)
    {
	    m_tail = (m_tail + 1) % m_queue_capacity;
		m_queue[m_tail] = e;
    	++m_queue_size;       
    }
    
    const IEvent* front()
    {
        m_head = (m_head +1 ) % m_queue_capacity;
        --m_queue_size;
        return m_queue[m_head];
     }

private:
    const IEvent** m_queue;
    ThreadLock m_lock;
    ThreadCond m_produce_cond;
    ThreadCond m_consume_cond;
    
    size_t m_queue_capacity;
    size_t m_queue_size;
    int m_head;
    int m_tail;
};



}//end namespace common
}//end namespace zonda

#endif //ZONDA_COMMON_EVENTQUEUE_H_

