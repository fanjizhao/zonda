//File Name:    BufferTransport.h

#ifndef ZONDA_COMMON_RPC_BUFFER_TRANSPORT_H_
#define ZONDA_COMMON_RPC_BUFFER_TRANSPORT_H_


#include <iostream>
#include <transport/TTransport.h>

namespace zonda
{
namespace common
{
    
class BufferTransport : public apache::thrift::transport::TTransport
{
public:
    BufferTransport(uint8_t* buff, uint32_t buff_len);
    ~BufferTransport();
    virtual bool isOpen() { return true; }
    virtual void open() {}
    virtual void close() {}
    virtual void write_virt(const uint8_t*  buf, uint32_t len);   
    virtual uint32_t writeEnd();

    virtual uint32_t read_virt(uint8_t* buf, uint32_t len);
    virtual uint32_t readEnd();
    virtual uint32_t readAll_virt(uint8_t* buf, uint32_t len);

    
    //Non-Thrift functions        
    void start_read();
    void start_write();
    uint8_t* get_buff();
    uint32_t get_data_len();
     

    
private:
    //char m_buff[1048576]; //1MB
    uint8_t* m_buff;
    uint32_t m_buff_len;
    uint32_t m_data_len;
    uint32_t m_pos;
};

}//namespace common
}//namespace zonda

#endif //ZONDA_COMMON_RPC_BUFFER_TRANSPORT_H_


