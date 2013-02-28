#include "GlobalTimer.h"
#include "log/Logger.h"

namespace zonda
{
namespace common
{
GlobalTimerImpl::GlobalTimerImpl():
    m_init(false)
{
}
GlobalTimerImpl::~GlobalTimerImpl()
{
}
int GlobalTimerImpl::init(size_t max_timer_count)
{
    if (m_init) 
    {
        //LOG_DEBUG("Disallow to init twice");
        return 0;
    }
    int r = 0;
    r = m_timer.init(max_timer_count);
    if (r != 0)
    {
        LOG_FATAL("Failed to init GlobalTimer");
    }
    
    r = m_timer.start();
    if (r != 0)
    {
        LOG_FATAL("Failed to start GlobalTimer");
    }
    m_init = true;
    return 0;  
}
void GlobalTimerImpl::set_default_handler_sink(IEventSink* handler_sink)
{
    return;
}

int GlobalTimerImpl::start()
{
    return 0;
}

void GlobalTimerImpl::stop()
{
    
}

void GlobalTimerImpl::cancel_timer(int timer_id)
{
    m_timer.cancel_timer(timer_id);
}

void GlobalTimerImpl::cancel_all_timer()
{
    
}

    //!To set a timer, a timer event will be thrown into the handler_sink when the timer times out.
int GlobalTimerImpl::schedule(IEventSink* handler_sink, int64_t param, int delay, int interval)
{
    return m_timer.schedule(handler_sink, param, delay, interval);
} 

int GlobalTimerImpl::schedule(int64_t param, int delay, int interval)
{
    return 0;
}
    
}//namespace common
}//namespace zonda

