#ifndef ZONDA_COMMON_TRANS_IN_EVENT_H_
#define ZONDA_COMMON_TRANS_IN_EVENT_H_

#include <stdint.h>
#include <string>
#include "seda/IEventHandler.h"
#include "ITransService.h"

namespace zonda
{
namespace common
{
    
class TransInEvent: public IEvent
{
public:
    //!This buff will be allocated when destroy this object
    TransInEvent(uint8_t* buff, int buff_len);
    virtual int get_type() const;
    uint8_t* get_buff() const;
    int get_buff_len() const;
    void set_dst_addr(const char* ip, uint16_t port);
    void set_dst_addr(const char* ipc_addr);
    const char* get_dst_ip() const;
    uint16_t get_dst_port() const;
    const char* get_dst_ipc() const;
    
    ~TransInEvent();
private:
    uint8_t *m_buff;
    int m_buff_len;
    char m_ip[16];
    uint16_t m_port;
    char m_ipc_addr[100];    
};
}//namespace common;
}//namespace zonda;

#endif //ZONDA_COMMON_TRANS_IN_EVENT_H_

