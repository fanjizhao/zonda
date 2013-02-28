#ifndef ZONDA_COMMON_ZMQ_TRANS_SENDER_H_
#define ZONDA_COMMON_ZMQ_TRANS_SENDER_H_

#include <stdint.h>
#include <string>
#include <tr1/unordered_map>
#include "seda/IEventHandler.h"
#include "seda/IEventSink.h"
#include "seda/Stage.h"
#include "ITransSender.h"

namespace zonda
{
namespace common
{

using namespace std;
using namespace std::tr1;




class ZMQSender: public ITransSender
{
public:
    union ChannelID
    {
        int64_t id;
        struct
        {
            int32_t ip;
            uint16_t port;
        };
    };
public:
    ZMQSender();
    ~ZMQSender();
    virtual int init(void* param);
    //!If call this function, must call it before init
    virtual int send_data(int64_t call_id, uint8_t* buff, int data_len, const char* ip, uint16_t port);
    virtual int send_data(int64_t call_id, uint8_t* buff, int data_len, const char* ipc_addr);

private:
    void* get_conn(const char* ip, uint16_t port);
    void* get_conn(const char* ipc_addr);
    void remove_conn(const char* ip, uint16_t port);
    void remove_conn(const char* ipc_addr);
    
    typedef unordered_map<int64_t, void*> TCP_CONN_MAP;
    typedef unordered_map<string, void*> IPC_CONN_MAP;
    
    TCP_CONN_MAP    m_tcp_conn_map;
    IPC_CONN_MAP    m_ipc_conn_map;
    
    
public:    
    ///////////////////////////
    int64_t call_list[10000];
    struct timeval time_list[10000];
    int send_num;
    struct timeval t1;
    
    void output_stat();
private:
    int do_send(void* socket, uint8_t* buff, int data_len);
public:
    static void* m_zmq_context;

};

}//namespace common
}//namespace zonda

#endif //ZONDA_COMMON_ZMQ_TRANS_SENDER_H_


