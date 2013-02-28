#ifndef ZONDA_RPC_IMSG_FACTORY_H_
#define ZONDA_RPC_IMSG_FACTORY_H_

#include <stdint.h>
#include "IMsg.h"
namespace zonda
{
namespace common
{
    

class IMsgFactory
{
public:
    //!Serialize the msg and free the msg if not keep_msg.
    virtual int serialize_body(uint8_t* buff, int buff_len, 
        IMsg *d, int& data_len, bool keep_msg) = 0;
    //!Deserialize the msg and return the msg
    virtual IMsg* deserialize_body(uint8_t* buff, int buff_len, int msg_type) = 0;
    
    virtual ~IMsgFactory() {}
};

}//namespace common
}//namespace zonda

#endif //ZONDA_RPC_IMSG_FACTORY_H_


