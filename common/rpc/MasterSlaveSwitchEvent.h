#ifndef ZONDA_COMMON_MASTER_SLAVE_SWITCH_EVENT_H_
#define ZONDA_COMMON_MASTER_SLAVE_SWITCH_EVENT_H_

#include "seda/IEvent.h"
#include "seda/EventType.h"
#include "ServiceInstanceRole.h"

namespace zonda
{
namespace common
{

//!This event will be thrown if a service instance's role
//!changes, into the stage which offers this service instance.
class MasterSlaveSwitchEvent: public IEvent
{
public:
    virtual int get_type() const
    {
        return EventType::MASTER_SLAVE_SWITCH_EVENT;
    }
    
    
    uint16_t service_type;
    
    int old_role;
    int new_role;
};

} //end namespace common
} //end namespace zonda


#endif //ZONDA_COMMON_MASTER_SLAVE_SWITCH_EVENT_H_

