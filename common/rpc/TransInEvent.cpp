#include <strings.h>
#include <cstring>
#include "seda/EventType.h"
#include "TransInEvent.h"
#include "log/Logger.h"
namespace zonda
{
namespace common
{

TransInEvent::TransInEvent(uint8_t* buff, int len):
    m_buff(buff), 
    m_buff_len(len),
    m_port(0)
{    
    bzero(m_ip, 16);
    bzero(m_ipc_addr, 100);    
}

TransInEvent::~TransInEvent()
{
    delete [] m_buff;
}


int TransInEvent::get_type() const
{
    return EventType::TRANS_IN_EVENT;
}

uint8_t* TransInEvent::get_buff() const
{
    return m_buff;
}

int TransInEvent::get_buff_len() const
{
    return m_buff_len;
}

void TransInEvent::set_dst_addr(const char* ip, uint16_t port)
{
    strcpy(m_ip, ip);
    m_port = port;
}

void TransInEvent::set_dst_addr(const char* ipc_addr)
{
    strcpy(m_ipc_addr, ipc_addr);
}

const char* TransInEvent::get_dst_ip() const
{
    return m_ip;
}

uint16_t TransInEvent::get_dst_port() const
{
    return m_port;
}

const char* TransInEvent::get_dst_ipc() const
{
    return m_ipc_addr;
}

    
}//namespace common;
}//namespace zonda;


