#ifndef ZONDA_COMMON_RPC_HEAD_H_
#define ZONDA_COMMON_RPC_HEAD_H_

#include <arpa/inet.h>
#include <cstring>
#include "ServiceAddr.h"


namespace zonda
{
namespace common
{
    
struct RpcResCode
{
    enum
    {
        //! Success
        SUCCESS = 0,
        //! Timeout
        TIMEOUT = 1,
        //! Failed to send
        SEND_FAIL = 2,
        //! Msg size is over the limit
        MSG_TOO_BIG = 3
    };
};


class RpcHead
{
public:
	int32_t msg_type;
	ServiceAddr responser;
	ServiceAddr invoker;	
	int64_t transaction_id;
	int64_t trans_param;
	int64_t call_id;
	uint16_t biz_handler_id;
	int16_t res_code;

public:
	RpcHead():
    	msg_type(0),
    	responser(),
    	invoker(),
    	transaction_id(0),
    	trans_param(0),
    	call_id(0),
    	biz_handler_id(0),
    	res_code(0)
    {
        //LOG_DEBUG("RpcHead():" << reinterpret_cast<int64_t>(this));
    }
    ~RpcHead()
    {
        //LOG_DEBUG("~RpcHead():" << reinterpret_cast<int64_t>(this));
    }
    void hton()
    {
        msg_type = htonl(msg_type);
        responser.hton();
        invoker.hton();
        transaction_id = my_htonll(transaction_id);
        trans_param = my_htonll(trans_param);
        call_id = my_htonll(call_id);
        biz_handler_id= htons(biz_handler_id);
        res_code = htons(res_code);
    }
    void ntoh()
    {
        msg_type = ntohl(msg_type);
        responser.ntoh();
        invoker.ntoh();
        transaction_id = my_ntohll(transaction_id);
        trans_param = my_ntohll(trans_param);
        call_id = my_ntohll(call_id);
        biz_handler_id= ntohs(biz_handler_id);
        res_code = ntohs(res_code);
    }
    
    //!The object is still host byte order
    void hton_serialize(uint8_t* buff)
    {
        memcpy(buff, this, sizeof(RpcHead));
        RpcHead* rpc_head = reinterpret_cast<RpcHead*>(buff);
        rpc_head->hton();
    }

    //!Deserialize from a buff(big-endian, network byte order)
    //!The object after deserialization will be host byte order
    //!The object is host byte order   
    void ntoh_deserialize(uint8_t* buff)
    {
        memcpy(this, buff, sizeof(RpcHead));
        this->ntoh();
    }    
};

}//namespace common
}//namespace zonda

#endif //ZONDA_COMMON_RPC_HEAD_H_

