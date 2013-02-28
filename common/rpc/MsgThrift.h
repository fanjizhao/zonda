#ifndef ZONDA_RPC_MSGTHRIFT_H_
#define ZONDA_RPC_MSGTHRIFT_H_

#include <protocol/TJSONProtocol.h>
#include <protocol/TBinaryProtocol.h>
#include "BufferTransport.h"
#include "IMsg.h"
#include "util/MsgPool.h"
namespace zonda
{
namespace common
{
template<class MsgType, int TypeValue>
class MsgThrift : public IMsg, public MsgType
{
public:
    template<class T> friend class MsgPool;
	static MsgThrift<MsgType, TypeValue>* create();
	//virtual ~MsgThrift() throw() {}
    virtual int get_msg_type() const {return TypeValue;  }
    virtual int get_op_type() const {return TypeValue > 0 ? TypeValue: (-1*TypeValue);};
    virtual int serialize(uint8_t* buff, int buff_len, int& data_len);
    virtual int deserialize(uint8_t* buff, int buff_len);
    virtual void free();
protected:
    MsgThrift(){};

};


template<class MsgType, int TypeValue>
MsgThrift<MsgType, TypeValue>* MsgThrift<MsgType, TypeValue>::create()
{
    return MsgPool< MsgThrift<MsgType, TypeValue> >::get_msg();
}

template<class MsgType, int TypeValue>
void MsgThrift<MsgType, TypeValue>::free()
{
    MsgPool< MsgThrift<MsgType, TypeValue> >::free_msg(this);
}

template<class MsgType, int TypeValue>
int MsgThrift<MsgType, TypeValue>::serialize(uint8_t* buff, int buff_len, int& data_len)
{
    boost::shared_ptr<BufferTransport> my_transport(new BufferTransport(buff, buff_len));
    //apache::thrift::protocol::TJSONProtocol my_protocol(my_transport);
    apache::thrift::protocol::TBinaryProtocol my_protocol(my_transport);
    try
    {
        this->write(&my_protocol);
    }
    catch(apache::thrift::transport::TTransportException& e)
    {
        return -1;
    }
    data_len = my_transport->get_data_len();
    return 0;
}

template<class MsgType, int TypeValue>
int MsgThrift<MsgType, TypeValue>::deserialize(uint8_t* buff, int buff_len)
{
    boost::shared_ptr<BufferTransport> my_transport(new BufferTransport(buff, buff_len));
    //apache::thrift::protocol::TJSONProtocol my_protocol(my_transport);
    apache::thrift::protocol::TBinaryProtocol my_protocol(my_transport);
    //TODO: Catch the exception of write
    my_transport->start_read();
    this->read(&my_protocol);
    return 0;
}

}//namespace common
}//namespace zonda


#endif //ZONDA_RPC_MSGTHRIFT_H_


