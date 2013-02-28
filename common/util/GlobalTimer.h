#ifndef ZONDA_COMMON_GLOBAL_TIMER_H_
#define ZONDA_COMMON_GLOBAL_TIMER_H_

#include "Singleton.h"
#include "TimerService.h"

namespace zonda
{
namespace common
{
    
class GlobalTimerImpl: public ITimerService
{
public:
    int init(size_t max_timer_count = 100000);
    virtual void cancel_timer(int timer_id);


    //!To set a timer, a timer event will be thrown into the handler_sink when the timer times out.
    virtual int schedule(IEventSink* handler_sink, int64_t param, int delay, int interval = 0);    
protected:
    virtual void cancel_all_timer();
    virtual int schedule(int64_t param, int delay, int interval = 0);
    virtual void set_default_handler_sink(IEventSink* handler_sink);
    virtual int start();
    virtual void stop();
    GlobalTimerImpl();
    ~GlobalTimerImpl();
    bool m_init;
    TimerService m_timer; 
};

typedef Singleton<GlobalTimerImpl> GlobalTimer;


}//namespace common
}//namespace zonda

#endif //ZONDA_COMMON_GLOBAL_TIMER_H_

