#ifndef ZONDA_COMMON_TIMER_EVENT_H_
#define ZONDA_COMMON_TIMER_EVENT_H_

#include <stdint.h>
#include "seda/IEvent.h"

namespace zonda
{
namespace common
{

class TimerEvent: public IEvent
{
public:
    TimerEvent(int id, int64_t param);
    virtual int get_type() const;
    int get_id() const;
    int64_t get_param() const;
    ~TimerEvent() {};
private:
	int m_id;
	int64_t m_param;
};




}//namespace common
}//namespace zonda

#endif //ZONDA_COMMON_TIMER_EVENT_H_
