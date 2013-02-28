#ifndef ZONDA_COMMON_EVENT_DISPATCHER_H_
#define ZONDA_COMMON_EVENT_DISPATCHER_H_


#include "IEventQueue.h"
#include "IEvent.h"
#include "thread/ThreadLock.h"
#include "thread/ThreadCond.h"

namespace zonda
{
namespace common
{
using namespace std;

 
class EventDispatcher: public IEventSink
{
public:
    EventDispatcher();
    ~EventDispatcher();
    
    //! Put an event into the queue, if the queue is full, 
    //! this function will block until the queue is not full
    //! and put the event into the queue.
    //! If want a different dispatching strategy, please overload
    //! this function
    virtual void enqueue(const IEvent* event);
    
    //! Get the capacity of the queue
    virtual size_t size();
    
    void set_internal_queues(IEventQueue** queues, int queue_num);

private:
    IEventQueue** m_internal_queues;
    int m_internal_queue_num;
    unsigned int m_queue_index; 
};



}//end namespace common
}//end namespace zonda

#endif //ZONDA_COMMON_EVENT_DISPATCHER_H_

