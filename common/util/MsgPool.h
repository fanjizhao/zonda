#ifndef ZONDA_COMMON_UTIL_MSG_POOL_H_
#define ZONDA_COMMON_UTIL_MSG_POOL_H_

#include "rpc/IMsg.h"

namespace zonda
{
namespace common
{
    
template<class MsgType>
class MsgPool
{
public:
    static MsgType* get_msg()
    {
        return new MsgType();
    }
    static void free_msg(IMsg* msg)
    {
        delete msg;
    }
};

}//namespace common
}//namespace zonda

#endif //ZONDA_COMMON_UTIL_MSG_POOL_H_
