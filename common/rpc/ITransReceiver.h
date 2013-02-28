#ifndef ZONDA_COMMON_ITRANS_RECEIVER_H_
#define ZONDA_COMMON_ITRANS_RECEIVER_H_

#include <string>
#include <vector>
#include <utility>
#include <set>
#include "rpc/IMsgFactory.h"

namespace zonda
{
namespace common
{

class CallOutRecord;  
struct TransReceiverParm
{
    TransReceiverParm():
        start_port(5000), 
        port_count(1000),
        auto_listen_tcp(true)
    {}
    std::vector<std::pair<std::string,uint16_t> > tcp_addr_list;
    std::vector<std::string> ipc_addr_list;
    
    //!When the trans received a data, it will throw an msg in
    vector<CallOutRecord*> call_out_record_list;
    uint16_t start_port;
    int port_count;
    bool auto_listen_tcp;
    IMsgFactory* msg_factory;
};

    
class ITransReceiver
{
public:
    virtual int init(void* parm) = 0;
    virtual int start() = 0;
    virtual void stop() = 0;
    virtual void get_listen_addr(
        std::vector<std::pair<std::string,uint16_t> >& tcp_addr_list, 
        std::vector<std::string>& ipc_addr_list) = 0;
    virtual void get_listen_addr(
        std::set<std::pair<std::string,uint16_t> >& tcp_addr_set, 
        std::set<std::string>& ipc_addr_set) = 0;
    virtual ~ITransReceiver() {};
};

}//namespace common
}//namespace zonda

#endif //ZONDA_COMMON_ITRANS_RECEIVER_H_


