#ifndef ZONDA_RPC_IMSG_H_
#define ZONDA_RPC_IMSG_H_

#include <stdint.h>

namespace zonda
{
namespace common
{


    
class IMsg
{
public:
    template<class T> friend class MsgPool;
    virtual int get_msg_type() const = 0;
    virtual int get_op_type() const  = 0;
    virtual int serialize(uint8_t* buff, int buff_len, int& data_len) = 0;
    virtual int deserialize(uint8_t* buff, int buff_len) = 0;
    virtual void free() = 0;
protected:
    virtual ~IMsg() throw() {}
};

}//namespace common
}//namespace zonda

#endif //ZONDA_RPC_IMSG_H_


