#include <algorithm>
#include <zmq.h>
#include <cstring>
#include <fstream>
#include "ZMQSender.h"
#include "seda/EventType.h"
#include "TransInEvent.h"
#include "util/net_util.h"
#include "util/string_ex.h"

using namespace zonda::common;

void* ZMQSender::m_zmq_context = NULL;


ZMQSender::ZMQSender()
{
    //m_zmq_context = NULL;
    send_num = 0;
}

ZMQSender::~ZMQSender()
{
    TCP_CONN_MAP::iterator it;
    for (it = m_tcp_conn_map.begin(); it != m_tcp_conn_map.end(); ++it)
    {
        zmq_close(it->second);
    }
    
    IPC_CONN_MAP::iterator it2;
    for (it2 = m_ipc_conn_map.begin(); it2 != m_ipc_conn_map.end(); ++it2)
    {
        zmq_close(it2->second);
    }    
    
    zmq_term(m_zmq_context);
    m_zmq_context = NULL;
}

int ZMQSender::init(void* param)
{
    LOG_DEBUG("m_zmq_context=" << ZMQSender::m_zmq_context);
    /*
    if (! (m_zmq_context = zmq_init(1)) )
    {
        LOG_FATAL("Failed to call zmq_init, error:" << zmq_strerror(errno));
        return -1;
    }*/
    return 0;
}


void* ZMQSender::get_conn(const char* ip, uint16_t port)
{
    ChannelID channel;
    
    channel.id = 0;
    struct sockaddr_in sa;
    sa.sin_addr.s_addr = 0;
    inet_pton(AF_INET, ip, &(sa.sin_addr)); 
    channel.ip = sa.sin_addr.s_addr;
    channel.port = port;
    
    TCP_CONN_MAP::iterator it;
    it = m_tcp_conn_map.find(channel.id);
    if (it != m_tcp_conn_map.end())
    {
        LOG_DEBUG("Get connection to " << ip << ":" << port
            << " from the tcp_conn_map");
        return it->second;        
    }
    
    
    // Socket to talk to server
    void * socket = zmq_socket(m_zmq_context, ZMQ_PUSH);
    if (socket == NULL)
    {
        LOG_ERROR("Failed to call zmq_socket. Error:" << zmq_strerror(errno)
            << " ip:" << ip << ", port:" << port);
        return NULL;
    }
    
    int r = 0;
    char addr[100];
    sprintf(addr, "tcp://%s:%d", ip, port);
    r = zmq_connect (socket, addr);
    if (r == 0)
    {
        LOG_INFO("Successfully connected to : " << addr);
        m_tcp_conn_map[channel.id] = socket;
        return socket;
    }

    LOG_ERROR("Failed to connect " << addr
         <<". Error:" << zmq_strerror(errno));
    return NULL;
     
}

void* ZMQSender::get_conn(const char* ipc_addr)
{
    char addr[100];
    sprintf(addr, "ipc://%s", ipc_addr);
        
    IPC_CONN_MAP::iterator it;
    it = m_ipc_conn_map.find(addr);
    if (it != m_ipc_conn_map.end())
    {
        LOG_DEBUG("Get connection to " << addr
            << " from the ipc_conn_map");
        return it->second;        
    }
    
    
    // Socket to talk to server
    void * socket = zmq_socket(m_zmq_context, ZMQ_PUSH);
    if (socket == NULL)
    {
        LOG_ERROR("Failed to call zmq_socket. Error:" << zmq_strerror(errno)
            << addr);
        return NULL;
    }
    
    int r = 0;

    r = zmq_connect (socket, addr);
    if (r == 0)
    {
        LOG_INFO("Successfully connected to : " << addr);
        m_ipc_conn_map[addr] = socket;
        return socket;
    }

    LOG_ERROR("Failed to connect " << addr
         <<". Error:" << zmq_strerror(errno));
    return NULL;

}

void ZMQSender::remove_conn(const char* ip, uint16_t port)
{
    ChannelID channel;
    
    channel.id = 0;
    struct sockaddr_in sa;
    sa.sin_addr.s_addr = 0;
    inet_pton(AF_INET, ip, &(sa.sin_addr)); 
    channel.ip = sa.sin_addr.s_addr;
    channel.port = port;
    
    TCP_CONN_MAP::iterator it;
    it = m_tcp_conn_map.find(channel.id);
    if (it != m_tcp_conn_map.end())
    {
        LOG_DEBUG("remove connection to " << ip << ":" << port
            << " from the tcp_conn_map");
        zmq_close(it->second);
        m_tcp_conn_map.erase(it);        
    }
}

void ZMQSender::remove_conn(const char* ipc_addr)
{
    char addr[100];
    sprintf(addr, "ipc://%s", ipc_addr);
    
    IPC_CONN_MAP::iterator it;
    it = m_ipc_conn_map.find(addr);
    if (it != m_ipc_conn_map.end())
    {
        LOG_DEBUG("remove connection to " << addr
            << " from the ipc_conn_map");
        zmq_close(it->second);
        m_ipc_conn_map.erase(it);        
    }      
}

void ZMQSender::output_stat()
{
    char buff[1204];
    sprintf(buff, "send_time_%ld.txt", Thread::self_id());
    std::ofstream outfile (buff);
    for (int i=0; i<send_num; ++i)
    {
        sprintf(buff, "%ld\t%ld.%06ld\n",
            call_list[i], time_list[i].tv_sec, time_list[i].tv_usec);
            
        outfile << buff;
    }
    outfile.close();
    //exit(0);    
}

int ZMQSender::send_data(int64_t call_id, 
    uint8_t* buff, int data_len, 
    const char* ip, uint16_t port)
{
    int r = 0;
    void* socket = NULL;
    socket = get_conn(ip, port);
    if (socket == NULL)
    {
        LOG_ERROR("Failed to get socket connecting to " 
            << ip << ":" 
            << port);
        return -1;
    }
    LOG_INFO("call_id:" << call_id
        << ", will send to " << ip
        << ":" << port
        << ", data len:" << data_len);       
    
    //gettimeofday(&t1, NULL);        
    r = do_send(socket, buff, data_len);
    //call_list[send_num] = call_id;
    //time_list[send_num] = t1;
    //++send_num;
    
    if (r != 0)
    {
        remove_conn(ip, port);
        return -1;
    }
    LOG_DEBUG("call_id:" << call_id << " msg has been sent"); 
    

    return 0;

}

int ZMQSender::send_data(int64_t call_id, 
    uint8_t* buff, int data_len, const char* ipc_addr)
{
    int r = 0;
    void* socket = NULL;
    socket = get_conn(ipc_addr);    
    if (socket == NULL)
    {
        LOG_ERROR("Failed to get socket connecting to " 
            << ipc_addr);
        return -1;  
    }
    LOG_INFO("call_id:" << call_id
        << ", will send to " << ipc_addr
        << ", data len:" << data_len);
    
    
    r = do_send(socket, buff, data_len);
    if (r != 0)
    {
        remove_conn(ipc_addr);
        return -1;
    }
    LOG_DEBUG("call_id:" << call_id << "msg has been sent"); 
    return 0;
}
        
int ZMQSender::do_send(void* socket, uint8_t* buff, int data_len)
{
    int r = 0;
   
    zmq_msg_t request;
    r = zmq_msg_init_size (&request, data_len);
    if (r != 0)
    {
        LOG_ERROR("Failed to call zmq_msg_init_size, size:" 
            << data_len << ", Error: " 
            <<zmq_strerror(errno));
        return -1;
        
    }
    memcpy (zmq_msg_data (&request), buff, data_len);
    
    while (1)
    {
        r = zmq_send(socket, &request, 0);
        if (errno == EINTR)
        {
            LOG_DEBUG("EINTR, retry");
            continue;
        }
        break;
    }
    zmq_msg_close(&request);
    
    if (r != 0)
    {
        LOG_ERROR("Failed to call zmq_msg_send, " 
            "Error: " <<zmq_strerror(errno));
        return -1;     
    }
    return 0;    
}

//
////////////////////////////////////

    
    


