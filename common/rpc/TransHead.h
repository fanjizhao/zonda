#ifndef ZONDA_COMMON_TRANS_HEAD_H_
#define ZONDA_COMMON_TRANS_HEAD_H_

#include <arpa/inet.h>
#include <cstring>
#include "util/byte_order.h"

namespace zonda
{
namespace common
{

class TransHead
{
public:
    int32_t version;
    int32_t rpc_head_len;
    int32_t rpc_msg_len;
    int64_t call_id;
    void hton()
    {
        version = htonl(version);
        rpc_head_len = htonl(rpc_head_len);
        rpc_msg_len = htonl(rpc_msg_len);
        call_id = my_htonll(call_id);
    }
    void ntoh()
    {
        version = ntohl(version);
        rpc_head_len = ntohl(rpc_head_len);
        rpc_msg_len = ntohl(rpc_msg_len);
        call_id = my_ntohll(call_id);        
    }
    
    //!Serialize the data into a buff
    //!The data serialized is big-endian(network byte order)
    //!The object is still host byte order
    void hton_serialize(uint8_t* buff)
    {
        memcpy(buff, this, sizeof(TransHead));
        TransHead* head = reinterpret_cast<TransHead*>(buff);
        head->hton();      
    }

    //!Deserialize from a buff(big-endian, network byte order)
    //!The object after deserialization will be host byte order
    //!The object is host byte order   
    void ntoh_deserialize(uint8_t* buff)
    {
        memcpy(this, buff, sizeof(TransHead));
        this->ntoh();
    }
};

}//namespace common
}//namespace zonda

#endif //ZONDA_COMMON_TRANS_HEAD_H_


