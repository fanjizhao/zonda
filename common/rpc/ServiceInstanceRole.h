#ifndef ZONDA_COMMON_SERVICE_INSTANCE_ROLE_H_
#define ZONDA_COMMON_SERVICE_INSTANCE_ROLE_H_

#include "seda/IEvent.h"
#include "seda/EventType.h"

namespace zonda
{
namespace common
{

class ServiceInstanceRole
{
public:
    enum
    {
        UNKNOWN = -1,
        MASTER = 0,
        SLAVE = 1,
    };

    static const char* get_desc(int role);
private:    
    static const char* desc[3];
    
};


} //end namespace common
} //end namespace zonda


#endif //ZONDA_COMMON_SERVICE_INSTANCE_ROLE_H_

