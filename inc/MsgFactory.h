#ifndef ZONDA_INC_MSG_FACTORY_H_
#define ZONDA_INC_MSG_FACTORY_H_

#include "rpc/IMsgFactory.h"


namespace zonda
{
class MsgFactory: public zonda::common::IMsgFactory
{
public:
    MsgFactory();
    //!Serialize the msg and free the msg.
    int serialize_body(uint8_t* buff, int buff_len, zonda::common::IMsg *d, int& data_len, bool keep_msg);
    //!Deserialize the msg and return the msg
    zonda::common::IMsg* deserialize_body(uint8_t* buff, int buff_len, int msg_type);
};

} //namespace zonda
#endif //ZONDA_INC_MSG_FACTORY_H_

