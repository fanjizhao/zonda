#include <cstring>
#include "seda/EventType.h"
#include "TransOutEvent.h"
#include "log/Logger.h"
namespace zonda
{
namespace common
{

TransOutEvent::TransOutEvent(uint8_t* buff, int data_len, int64_t call_id):
    m_buff(buff), 
    m_data_len(data_len),
    m_port(0),
    m_call_id(call_id)
{    
    m_ip[0] = '\0';
    m_ipc_addr[0] = '\0';    
}

TransOutEvent::~TransOutEvent()
{
    delete [] m_buff;
}

void TransOutEvent::set_dst_addr(const char* ip, uint16_t port)
{
    strcpy(m_ip, ip);
    m_port = port;
}

void TransOutEvent::set_dst_addr(const char* ipc_addr)
{
    strcpy(m_ipc_addr, ipc_addr);
}

int TransOutEvent::get_type() const
{
    return EventType::TRANS_OUT_EVENT;
}

const uint8_t* TransOutEvent::get_buff() const
{
    return m_buff;
}

int TransOutEvent::get_data_len() const
{
    return m_data_len;
}

const char* TransOutEvent::get_dst_ip() const
{
    return m_ip;
}

uint16_t TransOutEvent::get_dst_port() const
{
    return m_port;
}

const char* TransOutEvent::get_dst_ipc() const
{
    return m_ipc_addr;
}

void TransOutEvent::set_call_id(int64_t call_id)
{
    m_call_id = call_id;
}

int64_t TransOutEvent::get_call_id() const
{
    return m_call_id;
}
    
        
}//namespace common;
}//namespace zonda;


