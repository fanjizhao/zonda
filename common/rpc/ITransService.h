#ifndef ZONDA_COMMON_ITRANS_SERVICE_H_
#define ZONDA_COMMON_ITRANS_SERVICE_H_

#include <string>
#include <vector>
#include <utility>
#include <set>

namespace zonda
{
namespace common
{
   
struct TransParm
{
    TransParm():
        recv_sink(NULL), 
        start_port(5000), 
        port_count(1000),
        auto_listen_tcp(true)
    {}
    std::vector<std::pair<std::string,uint16_t> > tcp_addr_list;
    std::vector<std::string> ipc_addr_list;
    
    //!When the trans received a data, it will throw an TransInEvent
    IEventSink* recv_sink;

    uint16_t start_port;
    int port_count;
    bool auto_listen_tcp;
};

    
class ITransService
{
public:
    virtual int init(void* parm) = 0;
    virtual int start() = 0;
    virtual void stop() = 0;
    virtual int send_data(int64_t call_id, uint8_t* buff, int len, const char* ip, uint16_t port) = 0;
    virtual int send_data(int64_t call_id, uint8_t* buff, int len, const char* ipc_addr) = 0;
    virtual void get_listen_addr(
        std::vector<std::pair<std::string,uint16_t> >& tcp_addr_list, 
        std::vector<std::string>& ipc_addr_list) = 0;
    virtual void get_listen_addr(
        std::set<std::pair<std::string,uint16_t> >& tcp_addr_set, 
        std::set<std::string>& ipc_addr_set) = 0;
    virtual ~ITransService() {};
};

}//namespace common
}//namespace zonda

#endif //ZONDA_COMMON_ITRANS_SERVICE_H_


