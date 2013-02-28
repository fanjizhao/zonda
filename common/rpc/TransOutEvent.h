#ifndef ZONDA_COMMON_TRANS_OUT_EVENT_H_
#define ZONDA_COMMON_TRANS_OUT_EVENT_H_

#include <stdint.h>
#include <string>
#include "seda/IEventHandler.h"
#include "ITransService.h"

namespace zonda
{
namespace common
{
    
class TransOutEvent: public IEvent
{
public:
    TransOutEvent(uint8_t* buff, int data_len, int64_t call_id);
    void set_dst_addr(const char* ip, uint16_t port);
    void set_dst_addr(const char* ipc_addr);
    const char* get_dst_ip() const;
    uint16_t get_dst_port() const;
    const char* get_dst_ipc() const;
    virtual int get_type() const;
    const uint8_t* get_buff() const;
    int get_data_len() const;
    void set_call_id(int64_t call_id);
    int64_t get_call_id() const; 
    ~TransOutEvent();
private:
    uint8_t *m_buff;
    int m_data_len;
    char m_ip[16];
    uint16_t m_port;
    char m_ipc_addr[100];
    int64_t m_call_id;
    
};
}//namespace common;
}//namespace zonda;

#endif //ZONDA_COMMON_TRANS_OUT_EVENT_H_

