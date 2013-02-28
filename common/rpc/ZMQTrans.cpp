#include <algorithm>
#include <zmq.h>
#include <cstring>
#include "ZMQTrans.h"
#include "seda/EventType.h"
#include "TransInEvent.h"
#include "util/net_util.h"
#include "util/string_ex.h"

namespace zonda
{
namespace common
{

ZMQSendStageHander::ZMQSendStageHander()
{
    m_zmq_context = NULL;
}

ZMQSendStageHander::~ZMQSendStageHander()
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

int ZMQSendStageHander::init(IStage* stage, void* param)
{
    if (! (m_zmq_context = zmq_init(1)) )
    {
        LOG_FATAL("Failed to call zmq_init, error:" << zmq_strerror(errno));
        return -1;
    }
    return 0;
}


int ZMQSendStageHander::handle_event(const IEvent* event)
{
    int event_type = event->get_type();
    LOG_DEBUG("Received a event, type:" << event_type);
    if (event_type == EventType::TRANS_OUT_EVENT)
    {
        return do_send(static_cast<const TransOutEvent*>(event));
    }
    
    return 0;
}
    
void* ZMQSendStageHander::get_conn(const char* ip, uint16_t port)
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

void* ZMQSendStageHander::get_conn(const char* ipc_addr)
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

void ZMQSendStageHander::remove_conn(const char* ip, uint16_t port)
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

void ZMQSendStageHander::remove_conn(const char* ipc_addr)
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
    
int ZMQSendStageHander::do_send(const TransOutEvent* event)
{
    int r = 0;
    void* socket = NULL;
    if (event->get_dst_port())
    {
        socket = get_conn(event->get_dst_ip(), event->get_dst_port());
        if (socket == NULL)
        {
            LOG_ERROR("Failed to get socket connecting to " 
                << event->get_dst_ip() << ":" 
                << event->get_dst_port());
            goto end;  
        }
        LOG_INFO("call_id:" << event->get_call_id() 
            << ", will send to " << event->get_dst_ip()
            << ":" << event->get_dst_port()
            << ", data len:" << event->get_data_len());        
    }
    else
    {
        socket = get_conn(event->get_dst_ipc());
        if (socket == NULL)
        {
            LOG_ERROR("Failed to get socket connecting to " 
                << event->get_dst_ipc());
            goto end;
        }        
        LOG_INFO("call_id:" << event->get_call_id() 
            << ", will send to ipc://" << event->get_dst_ipc()
            << ", data len:" << event->get_data_len());

    }
    

    
    zmq_msg_t request;
    r = zmq_msg_init_size (&request, event->get_data_len());
    if (r != 0)
    {
        LOG_ERROR("Failed to call zmq_msg_init_size, size:" 
            << event->get_data_len() << ", Error: " 
            <<zmq_strerror(errno));
        goto end;
        
    }
    memcpy (zmq_msg_data (&request), event->get_buff(), event->get_data_len());
    r = zmq_send(socket, &request, 0);
    if (r != 0)
    {
        LOG_ERROR("Failed to call zmq_msg_send, " 
            "Error: " <<zmq_strerror(errno));
        if (event->get_dst_port())
        {
            remove_conn(event->get_dst_ip(), event->get_dst_port());
        }
        else
        {
            remove_conn(event->get_dst_ipc());
        }
        goto end;            
    }
    zmq_msg_close(&request);
    LOG_DEBUG("call_id:" << event->get_call_id() << " msg has been sent"); 

    
end:
    //NOTE: Change the way of free Even 
    delete event;
    
    return 0;    
}

//
////////////////////////////////////

ZMQRecvThread::ZMQRecvThread()
{
    LOG_DEBUG("ZMQRecvThread()");
    m_zmq_context = NULL;
}

int ZMQRecvThread::init(void* param)
{
    TransParm* zmq_param = (TransParm*)param;
    m_recv_sink = zmq_param->recv_sink;
    
    if (! (m_zmq_context = zmq_init(1)) )
    {
        LOG_FATAL("Failed to call zmq_init, error:" << zmq_strerror(errno));
        return -1;
    }

    if (zmq_param->tcp_addr_list.size() > 0)
    {
        do_tcp_listen(zmq_param->tcp_addr_list);
    }
    else 
    {
        if (zmq_param->auto_listen_tcp)
        {
            do_auto_tcp_listen(zmq_param->start_port, zmq_param->port_count);
        }
    }
    
    do_ipc_listen(zmq_param->ipc_addr_list);
    
    //If no successful tcp addr binding, regard it as failure     
    if (m_zmq_socket_list.size() == 0)
    {
        return -1;
    }
        
    return 0;
}

int ZMQRecvThread::do_tcp_listen(const std::vector<std::pair<std::string,uint16_t> >& tcp_addr_list)
{
    void* zmq_sock = NULL;
    char addr[200];
    for (size_t i=0; i<tcp_addr_list.size(); ++i)
    {
        zmq_sock = zmq_socket(m_zmq_context, ZMQ_PULL);
        if (!zmq_sock)
        {
            LOG_ERROR("Failed to call zmq_socket, error:" << zmq_strerror(errno));
            return -1;            
        }

        sprintf(addr, "tcp://%s:%d", tcp_addr_list[i].first.c_str(), tcp_addr_list[i].second);
        LOG_INFO("zmq bind to " << addr);
        if (zmq_bind(zmq_sock, addr))
        {
            LOG_ERROR("Failed to call zmq_bind(" << addr
                << "), error:" << zmq_strerror(errno));
            return -1;
        }
        
        m_zmq_socket_list.push_back(zmq_sock);
        m_tcp_addr_list.push_back(tcp_addr_list[i]); 
    }
    
    return 0;
}



int ZMQRecvThread::do_auto_tcp_listen(uint16_t start_port, int port_count)
{
    void* zmq_sock = NULL;
    char addr[200];

    zmq_sock = zmq_socket(m_zmq_context, ZMQ_PULL);
    if (!zmq_sock)
    {
        LOG_ERROR("Failed to call zmq_socket, error:" << zmq_strerror(errno));
        return -1;            
    }
    
    for (uint16_t port = start_port; port < start_port+port_count; ++port)
    {
        sprintf(addr, "tcp://0.0.0.0:%d", port);
        LOG_DEBUG("zmq try to bind to " << addr);
        if (zmq_bind(zmq_sock, addr))
        {
            LOG_WARN("Failed to call zmq_bind(" << addr
                << "), error:" << zmq_strerror(errno));
        }
        else
        {
            LOG_INFO("zmq bind to " << addr);
            m_zmq_socket_list.push_back(zmq_sock);

            std::set<std::string> ip_set;
            int r = get_local_ip_addr(ip_set, true);
            if (r != 0)
            {
                LOG_ERROR("Failed to get local ip addr");
                return -1;
            }
            
            std::set<std::string>::iterator it;
            for (it = ip_set.begin(); it != ip_set.end(); ++it)
            {
                std::pair<std::string, uint16_t> tcp_addr;
                //str_to_tcp_addr(*it, tcp_addr);
                tcp_addr.first = *it;
                tcp_addr.second = port;
                m_tcp_addr_list.push_back(tcp_addr); 
            }
            return 0;
        }
    }    
    
    return -1;
}

int ZMQRecvThread::do_ipc_listen(const std::vector<std::string>& ipc_addr_list)
{
    void* zmq_sock = NULL;
    char addr[200];
    int r = 0;
    for (size_t i=0; i<ipc_addr_list.size(); ++i)
    {
        if (ipc_addr_list[i].find(",") != std::string::npos)
        {
            LOG_FATAL("Disallow the ipc addr contains commas. \""
                << ipc_addr_list[i] << "\"");
            return -1;
        }
        zmq_sock = zmq_socket(m_zmq_context, ZMQ_PULL);
        if (!zmq_sock)
        {
            LOG_ERROR("Failed to call zmq_socket, error:" << zmq_strerror(errno));
            return -1;            
        }

        sprintf(addr, "ipc://%s", ipc_addr_list[i].c_str());
        LOG_INFO("zmq bind to " << addr << ", socket:" << (int64_t)zmq_sock);
        if (zmq_bind(zmq_sock, addr))
        {
            LOG_ERROR("Failed to call zmq_bind(" << addr
                << "), error:" << zmq_strerror(errno));
            return -1;
        }
        
        r = chmod(ipc_addr_list[i].c_str(), S_IWOTH|S_IWGRP|S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH);
        if (r != 0)
        {
            LOG_FATAL("Failed to chmod " << ipc_addr_list[i].c_str()
                << ".Error: " << strerror(errno));
            return -1;
        }
        m_zmq_socket_list.push_back(zmq_sock);
        m_ipc_addr_list.push_back(ipc_addr_list[i]); 
    }
    
    return 0;    
}

void ZMQRecvThread::get_listen_addr(
        std::vector<std::pair<std::string,uint16_t> >& tcp_addr_list, 
        std::vector<std::string>& ipc_addr_list)
{
    tcp_addr_list = m_tcp_addr_list;
    ipc_addr_list = m_ipc_addr_list;
    
}

void ZMQRecvThread::get_listen_addr(
    std::set<std::pair<std::string,uint16_t> >& tcp_addr_set, 
    std::set<std::string>& ipc_addr_set)
{
    for (size_t i=0; i< m_tcp_addr_list.size(); ++i)
    {
        tcp_addr_set.insert(m_tcp_addr_list[i]);
    }

    for (size_t i=0; i< m_ipc_addr_list.size(); ++i)
    {
        ipc_addr_set.insert(m_ipc_addr_list[i]);
    }

}
        
void ZMQRecvThread::run()
{
    LOG_DEBUG("Begin...");
    zmq_pollitem_t items[m_zmq_socket_list.size()];
    for (size_t i=0; i<m_zmq_socket_list.size(); ++i)
    {
        items[i].socket = m_zmq_socket_list[i];
        items[i].events = ZMQ_POLLIN;
        //LOG_DEBUG("Push socket:" << (int64_t)items[i].socket << " into polling set");
    }
    
    int rc = 0;
    while (1) 
    {
        // Wait for next request from client
        zmq_msg_t request;
        rc = zmq_poll(items, m_zmq_socket_list.size(), -1);
        if (rc < 0)
        {
            LOG_ERROR("zmq_poll error:" << zmq_strerror(errno));
            continue;            
        }
        
        for (size_t i=0; i<m_zmq_socket_list.size(); ++i)
        {
            if (items[i].revents & ZMQ_POLLIN)
            {
                zmq_msg_init(&request);
                zmq_recv(items[i].socket, &request, 0);
                size_t msg_size = zmq_msg_size(&request);
                
                if (msg_size < 2)
                {
                    continue;
                }
                
                LOG_DEBUG("Received a msg, size:" << msg_size);
                
                if (m_recv_sink == NULL)
                {
                    LOG_WARN("recv_sink is NULL,discard the msg");
                    zmq_msg_close (&request); 
                    continue;
                }

                uint8_t* buff = new uint8_t[msg_size];
                memcpy(buff, zmq_msg_data(&request), msg_size);
                zmq_msg_close (&request);        
                
                if (m_recv_sink)
                {
                    TransInEvent* trans_in_event = new TransInEvent(buff, msg_size);
                    m_recv_sink->enqueue(trans_in_event);
                }
                else
                {
                    LOG_WARN("recv_sink is NULL, so discard the msg");
                    delete buff;
                }              
            }
        }


    }
}

void ZMQRecvThread::stop()
{
    //TODO: Send one-byte msg to the zmq socket
    //to make the loop in the run() exit
}
    
    
//ZMQTrans
///////////////////////////        
ZMQTrans::ZMQTrans(const std::string& trans_name, size_t queue_size):
    m_send_stage("ZMQTrans_Send", queue_size)
{
    m_recv_thread.set_thread_name("ZMQTrans_Recv");    
    m_send_sink = m_send_stage.get_sink();
}    

int ZMQTrans::init(void* param)
{
    //TransParm* zmq_param = (TransParm*)param;
    
    int r = 0;
    r = m_send_stage.init(NULL);
    if (r != 0)
    {
        LOG_FATAL("Failed to init trans send_stage");
        return r;
    }
    
    r = m_recv_thread.init(param);
    if (r != 0)
    {
        LOG_FATAL("Failed to init trans recv thread");
    }
    
    //m_send_stage.enable_trace();
    

    
    return r; 
}

int ZMQTrans::start()
{
    int r = m_send_stage.start();
    if (r != 0)
    {
        LOG_FATAL("Failed to start m_send_stage");
        return -1;
    }
    
    r = m_recv_thread.start();
    
    return r;
}

void ZMQTrans::stop()
{
    return m_send_stage.stop();
}

int ZMQTrans::send_data(int64_t call_id, uint8_t* buff, int data_len, const char* ip, uint16_t port)
{
    LOG_DEBUG("call_id:" << call_id << ", to " << ip << ":" << port);
    if (data_len == 0)
    {
        LOG_ERROR("Refused to send 0 byte len data");
        return -1;
    }
    TransOutEvent* event = new TransOutEvent(buff, data_len, call_id);
    event->set_dst_addr(ip, port);
    m_send_sink->enqueue(event);
    LOG_DEBUG("call_id:" << call_id << ", has put into the m_send_sink");
    return 0;
}

int ZMQTrans::send_data(int64_t call_id, uint8_t* buff, int data_len, const char* ipc_addr)
{
    if (data_len == 0)
    {
        LOG_ERROR("Refused to send 0 byte len data");
        return -1;
    }
    TransOutEvent* event = new TransOutEvent(buff, data_len, call_id);
    event->set_dst_addr(ipc_addr);
    m_send_sink->enqueue(event);
    return 0;
}

void ZMQTrans::get_listen_addr(
        std::vector<std::pair<std::string,uint16_t> >& tcp_addr_list, 
        std::vector<std::string>& ipc_addr_list)
{
    m_recv_thread.get_listen_addr(tcp_addr_list, ipc_addr_list);
}

void ZMQTrans::get_listen_addr(
    std::set<std::pair<std::string,uint16_t> >& tcp_addr_set, 
    std::set<std::string>& ipc_addr_set)
{
    m_recv_thread.get_listen_addr(tcp_addr_set, ipc_addr_set);
}

}//namespace common
}//namespace zonda

