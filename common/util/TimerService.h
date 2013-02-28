#ifndef ZONDA_COMMON_TIMER_SERVICE_H_
#define ZONDA_COMMON_TIMER_SERVICE_H_


#include <queue>
#include "ITimerService.h"
#include "TimerThread.h"

namespace zonda
{
namespace common
{
    
class TimerService: public ITimerService
{
public:
    virtual int init(size_t max_timer_count);
    virtual void set_default_handler_sink(IEventSink* handler_sink);
    virtual int start();
    virtual void stop();
    virtual void cancel_timer(int timer_id);
    virtual void cancel_all_timer();
    
    
    //!To set a timer, a timer event will be thrown into the default handlerSink when the timer times out.
    //!If the default handler_sink is not set, the timer events are discarded.
   	//!@param interval represents how frequently the timer will be triggered, the unit is millsecond.
    //!@retval -1 Failed to schedule a timer 
    //!@retval >0 the timer_id,  which can be used to identify the timer
    int schedule(int64_t param, int delay, int interval = 0);

   
    //!To set a timer, a timer event will be thrown into the handler_sink when the timer times out.
    int schedule(IEventSink* handler_sink, int64_t param, int delay, int interval = 0);
    
    ~TimerService(){};

private:
    TimerThread m_thread;
};



}//end namespace common
}//end namespace zonda

#endif //ZONDA_COMMON_TIMER_SERVICE_H_

