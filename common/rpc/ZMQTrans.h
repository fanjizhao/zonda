#ifndef ZONDA_COMMON_ZMQ_TRANS_H_
#define ZONDA_COMMON_ZMQ_TRANS_H_

#include <stdint.h>
#include <string>
#include <tr1/unordered_map>
#include "seda/IEventHandler.h"
#include "seda/IEventSink.h"
#include "seda/Stage.h"
#include "ITransService.h"
#include "TransOutEvent.h"

namespace zonda
{
namespace common
{

using namespace std;
using namespace std::tr1;


union ChannelID
{
    int64_t id;
    struct
    {
        int32_t ip;
        uint16_t port;
    };
};
   
class ZMQSendStageHander: public IEventHandler
{
public:
    ZMQSendStageHander();
    ~ZMQSendStageHander();
    virtual int init(IStage* stage, void* param);
    virtual int handle_event(const IEvent* event);
    
private:
    void* get_conn(const char* ip, uint16_t port);
    void* get_conn(const char* ipc_addr);
    void remove_conn(const char* ip, uint16_t port);
    void remove_conn(const char* ipc_addr);
    
    typedef unordered_map<int64_t, void*> TCP_CONN_MAP;
    typedef unordered_map<string, void*> IPC_CONN_MAP;
    
    TCP_CONN_MAP    m_tcp_conn_map;
    IPC_CONN_MAP    m_ipc_conn_map;

private:
    int do_send(const TransOutEvent* event);
    void* m_zmq_context;
    
};   


class ZMQRecvThread: public Thread
{
public:
    ZMQRecvThread();
    int init(void* param);
    void run();
    void stop();
    void get_listen_addr(
        std::vector<std::pair<std::string,uint16_t> >& tcp_addr_list, 
        std::vector<std::string>& ipc_addr_list);
    void get_listen_addr(
        std::set<std::pair<std::string,uint16_t> >& tcp_addr_set, 
        std::set<std::string>& ipc_addr_set);        
private:
    int do_tcp_listen(const std::vector<std::pair<std::string,uint16_t> >& tcp_addr_list);
    //!Automatically choose the first available port on 0.0.0.0
    int do_auto_tcp_listen(uint16_t start_port, int port_count);
    int do_ipc_listen(const std::vector<std::string>& ipc_addr_list);
        
private:
    void* m_zmq_context;
    std::vector<void*> m_zmq_socket_list;
    IEventSink* m_recv_sink;
    std::vector<std::pair<std::string,uint16_t> > m_tcp_addr_list;
    std::vector<std::string> m_ipc_addr_list;
};

    

class ZMQTrans: public ITransService
{
public:
    ZMQTrans(const std::string& trans_name, size_t queue_size = 100000);
    virtual int init(void* param);
    virtual int start();
    virtual void stop();
    virtual int send_data(int64_t call_id, uint8_t* buff, int data_len, const char* ip, uint16_t port);
    virtual int send_data(int64_t call_id, uint8_t* buff, int data_len, const char* ipc_addr);
    virtual void get_listen_addr(
        std::vector<std::pair<std::string,uint16_t> >& tcp_addr_list, 
        std::vector<std::string>& ipc_addr_list);
    virtual void get_listen_addr(
        std::set<std::pair<std::string,uint16_t> >& tcp_addr_set, 
        std::set<std::string>& ipc_addr_set);
private:
    Stage<ZMQSendStageHander, EventQueue> m_send_stage;
    IEventSink* m_send_sink;
    ZMQRecvThread m_recv_thread;  
};

}//namespace common
}//namespace zonda

#endif //ZONDA_COMMON_ZMQ_TRANS_H_


