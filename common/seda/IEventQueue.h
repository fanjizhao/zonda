#ifndef ZONDA_COMMON_IEVENTQUEUE_H_
#define ZONDA_COMMON_IEVENTQUEUE_H_

#include "IEventSink.h"
#include "IEventSource.h"

namespace zonda
{
namespace common
{
    
class IEventQueue: public IEventSink, public IEventSource
{
public:
    virtual int set_capacity(size_t capacity) = 0;
	virtual ~IEventQueue() {};
};



}//end namespace common
}//end namespace zonda

#endif //ZONDA_COMMON_IEVENTQUEUE_H_
