#ifndef ZONDA_COMMON_ITIMERSERVICE_H_
#define ZONDA_COMMON_ITIMERSERVICE_H_


#include <stdint.h>
#include "seda/IEventSink.h"


namespace zonda
{
namespace common
{

class ITimerService
{
public:
    virtual int init(size_t max_timer_count) = 0;
    virtual void set_default_handler_sink(IEventSink* handler_sink) = 0;
    virtual int start() = 0;
    virtual void stop() = 0;
    virtual void cancel_timer(int timer_id) = 0;
    virtual void cancel_all_timer() = 0;
    
    
    //!To set a timer, a timer event will be thrown into the default handlerSink when the timer times out.
    //!If the default handler_sink is not set, the timer events are discarded.
   	//!@param interval represents how frequently the timer will be triggered, the unit is millsecond.
    //!@retval -1 Failed to schedule a timer 
    //!@retval >0 the timer_id,  which can be used to identify the timer
    virtual int schedule(int64_t param, int delay, int interval = 0) = 0;

   
    //!To set a timer, a timer event will be thrown into the handler_sink when the timer times out.
    virtual int schedule(IEventSink* handler_sink, int64_t param, int delay, int interval = 0) =0;
    
    virtual ~ITimerService() {};
};


}//namespace server_platform
}//namespace zonda


#endif //ZONDA_COMMON_ITIMERSERVICE_H_

