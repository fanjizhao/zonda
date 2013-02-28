#include "ServiceInstanceRole.h"

namespace zonda
{
namespace common
{


const char* ServiceInstanceRole::desc[] = 
{
    "MASTER",
    "SLAVE",
    "UNKNOW"
};

const char* ServiceInstanceRole::get_desc(int role)
{
    if (role < 0) return desc[2];
    
    return desc[role];
}

} //end namespace common
} //end namespace zonda



