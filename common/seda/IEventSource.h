#ifndef ZONDA_COMMON_IEVENTSOURCE_H_
#define ZONDA_COMMON_IEVENTSOURCE_H_

#include "IEvent.h"

namespace zonda
{
    
namespace common
{
class IEventSource
{
public:

    //! Get the top element and remove the element from the queue, 
    //! if the queue is empty, this method will block until it
    //! get an event 
    virtual const IEvent* dequeue() = 0;
    
    //! The method must return within the millseconds.
    //! In other worlds, it blocks for millseconds at most.
    //! If millseconds is 0, the method returns immediately
    //! no matter whether if the queue is empty(return NULL) or not(return a real event).
    virtual const IEvent* time_dequeue(int millseconds) = 0;

    
    //! Try to get at most array_size elements from the queue.
    //! @return the real num got from the queue. 0 indicates timeout
    virtual size_t time_dequeue(int millseconds, 
        const IEvent* event_array[], const size_t array_size) = 0;
        
    virtual ~IEventSource() {}
};

}//end namespace common
} //end namespace

#endif //ZONDA_COMMON_IEVENTSOURCE_H_

