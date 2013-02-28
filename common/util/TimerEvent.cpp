
#include "TimerEvent.h"
#include "seda/EventType.h"

namespace zonda
{
namespace common
{

TimerEvent::TimerEvent(int id, int64_t param):
	m_id(id), m_param(param)
{
}

int TimerEvent::get_type() const
{
	return EventType::TIMER_EVENT;
}

int TimerEvent::get_id() const
{
	return m_id;
}

int64_t TimerEvent::get_param() const
{
	return m_param;
}



}//namespace common
}//namespace zonda

