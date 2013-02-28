#ifndef ZONDA_COMMON_ZMQ_RECEIVER_H_
#define ZONDA_COMMON_ZMQ_RECEIVER_H_

#include <stdint.h>
#include <string>
#include <tr1/unordered_map>
#include <zmq.h>
#include "seda/IEventHandler.h"
#include "seda/IEventSink.h"
#include "seda/Stage.h"
#include "ITransReceiver.h"
#include "rpc/RpcHead.h"
#include "rpc/IMsg.h"

namespace zonda
{
namespace common
{

using namespace std;
using namespace std::tr1;


class ZMQReceiverThread: public Thread
{
public:
    ZMQReceiverThread();
    int init(void* param);
    void run();
    void stop();
    void get_listen_addr(
        std::vector<std::pair<std::string,uint16_t> >& tcp_addr_list, 
        std::vector<std::string>& ipc_addr_list);
    void get_listen_addr(
        std::set<std::pair<std::string,uint16_t> >& tcp_addr_set, 
        std::set<std::string>& ipc_addr_set);
    
    void output_stat();
    int64_t call_list[10000];
    struct timeval time_list[10000];
    int received_num;    
private:
    int do_tcp_listen(const std::vector<std::pair<std::string,uint16_t> >& tcp_addr_list);
    //!Automatically choose the first available port on 0.0.0.0
    int do_auto_tcp_listen(uint16_t start_port, int port_count);
    int do_ipc_listen(const std::vector<std::string>& ipc_addr_list);
    void handle_incoming_msg(RpcHead* rpc_head, IMsg* msg);   
private:
    void* m_zmq_context;
    std::vector<void*> m_zmq_socket_list;
    vector<CallOutRecord*> m_call_out_record_list;
    std::vector<std::pair<std::string,uint16_t> > m_tcp_addr_list;
    std::vector<std::string> m_ipc_addr_list;
    IMsgFactory* m_msg_factory;
    ThreadLock m_lock;
    ThreadCond m_cond;
    zmq_pollitem_t* m_poll_items;
};

    

class ZMQReceiver: public ITransReceiver
{
public:
    virtual int init(void* parm);
    virtual int start();
    virtual void stop();
    virtual void get_listen_addr(
        std::vector<std::pair<std::string,uint16_t> >& tcp_addr_list, 
        std::vector<std::string>& ipc_addr_list);
    virtual void get_listen_addr(
        std::set<std::pair<std::string,uint16_t> >& tcp_addr_set, 
        std::set<std::string>& ipc_addr_set);
    virtual ~ZMQReceiver() {};
    void output_stat(){m_recv_thread.output_stat();}
private:    
    ZMQReceiverThread m_recv_thread;  
};

}//namespace common
}//namespace zonda

#endif //ZONDA_COMMON_ZMQ_RECEIVER_H_


