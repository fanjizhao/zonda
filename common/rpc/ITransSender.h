#ifndef ZONDA_COMMON_ITRANS_SENDER_H_
#define ZONDA_COMMON_ITRANS_SENDER_H_

#include <string>
#include <vector>
#include <utility>
#include <set>

namespace zonda
{
namespace common
{
   
   
class ITransSender
{
public:
    virtual int init(void* param) = 0;
    virtual int send_data(int64_t call_id, uint8_t* buff, int len, const char* ip, uint16_t port) = 0;
    virtual int send_data(int64_t call_id, uint8_t* buff, int len, const char* ipc_addr) = 0;
    virtual ~ITransSender() {};
};

}//namespace common
}//namespace zonda

#endif //ZONDA_COMMON_ITRANS_SENDER_H_


