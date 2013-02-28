
#include "TimerService.h"

namespace zonda
{
namespace common
{

int TimerService::init(size_t max_timer_count)
{

	return m_thread.init(max_timer_count);
}

void TimerService::set_default_handler_sink(IEventSink* handler_sink)
{
	m_thread.set_default_handler_sink(handler_sink);
}

int TimerService::start()
{
    m_thread.set_thread_name("TimerThread");    
	return m_thread.start();
}

void TimerService::stop()
{
	m_thread.stop();
}

void TimerService::cancel_timer(int timer_id)
{
	return m_thread.cancel_timer(timer_id);
}

void TimerService::cancel_all_timer()
{
	m_thread.cancel_all_timer();
} 

int TimerService::schedule(int64_t param, int delay, int interval)
{
	return m_thread.schedule(param, delay, interval);
}

int TimerService::schedule(IEventSink* handler_sink, int64_t param, int delay, int interval)
{
	return m_thread.schedule(handler_sink, param, delay, interval);
}

} //namespace common
} //namespace zonda

