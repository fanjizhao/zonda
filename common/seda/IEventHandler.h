#ifndef COMMON_IEVENT_HANDLER_H_
#define COMMON_IEVENT_HANDLER_H_

#include "IEvent.h"
#include "IStage.h"
namespace zonda
{
namespace common
{


//! Users must implement an event handler for each stage
struct IEventHandler
{
    virtual int init(IStage* stage, void* param) = 0;
    virtual int handle_event(const IEvent* event) = 0;
    virtual ~IEventHandler() {};
};

}//namespace common
}//namespace zonda

#endif //COMMON_IEVENT_HANDLER_H_

