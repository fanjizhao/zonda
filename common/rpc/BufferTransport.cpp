//File Name: BufferTransport.cpp


#include <cstring>
#include <cassert>
#include <iostream>
#include <transport/TTransportException.h>
#include "BufferTransport.h"

using namespace zonda::common;
using namespace std;

BufferTransport::BufferTransport(uint8_t* buff, uint32_t buff_len):
    m_buff(buff), 
    m_buff_len(buff_len),
    m_data_len(0),
    m_pos(0)
{

}

BufferTransport::~BufferTransport()
{

}

void BufferTransport::write_virt(const uint8_t*  buf, uint32_t len)
{
    if ((m_data_len + len) >= m_buff_len)
    {
        throw apache::thrift::transport::TTransportException(
            apache::thrift::transport::TTransportException::INTERNAL_ERROR);
    }
    memcpy(m_buff+m_data_len, buf, len);
    m_data_len += len;
}

void BufferTransport::start_read()
{
    m_pos = 0;
    //When read, the buff_len is the data_len
    m_data_len = m_buff_len;
}
void BufferTransport::start_write()
{

}

uint32_t BufferTransport::readEnd()
{
    std::cout << "pos:" << m_pos << std::endl;
    return 0;
}

uint32_t BufferTransport::writeEnd()
{    
    m_buff[m_data_len] = '\0';
    std::cout << m_buff << std::endl;
    std::cout << "data_len:" << m_data_len << std::endl;
    return 0;
}

uint32_t BufferTransport::read_virt(uint8_t* buf, uint32_t len) 
{
    assert(m_pos+len <= m_data_len);
    memcpy(buf, m_buff+m_pos, len);
    m_pos += len;
    return len;
}

uint32_t BufferTransport::readAll_virt(uint8_t* buf, uint32_t len) 
{
    //std::cout << "readAll_virt, len:" << len << std::endl;
    if (len <= m_data_len)
    {
        return this->read(buf, len);
    }
    else
    {
        throw apache::thrift::transport::TTransportException(
            apache::thrift::transport::TTransportException::INTERNAL_ERROR,
            "readAll, len>m_data_len");
    }
    
}


uint8_t* BufferTransport::get_buff()
{
    return m_buff;
}

uint32_t BufferTransport::get_data_len()
{
    return m_data_len;
}


