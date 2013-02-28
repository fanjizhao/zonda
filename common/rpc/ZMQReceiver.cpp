#include <algorithm>
#include <zmq.h>
#include <cstring>
#include <fstream>
#include "ZMQReceiver.h"
#include "seda/EventType.h"
#include "TransInEvent.h"
#include "util/net_util.h"
#include "util/string_ex.h"
#include "rpc/TransHead.h"
#include "rpc/ServiceStageMap.h"
#include "rpc/RpcEvent.h"
#include "rpc/CallOutRecord.h"

using namespace zonda::common;


ZMQReceiverThread::ZMQReceiverThread():
    m_cond(m_lock)
{
    LOG_DEBUG("ZMQReceiverThread()");
    m_zmq_context = NULL;
    received_num = 0;
    m_poll_items = NULL;
}

int ZMQReceiverThread::init(void* param)
{
    TransReceiverParm* zmq_param = (TransReceiverParm*)param;
    m_call_out_record_list = zmq_param->call_out_record_list;
    m_msg_factory = zmq_param->msg_factory;

    if (m_call_out_record_list.size() == 0)
    {
        LOG_ERROR("m_call_out_record_list is empty");
        return -1;                
    }   
    
        
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
    
    this->set_thread_name("ZMQReceiver");
    //Thread num is the same as the rpc stage handler num
    this->set_thread_num(m_call_out_record_list.size());

    m_poll_items = new zmq_pollitem_t[m_zmq_socket_list.size()];
    for (size_t i=0; i<m_zmq_socket_list.size(); ++i)
    {
        m_poll_items[i].socket = m_zmq_socket_list[i];
        m_poll_items[i].events = ZMQ_POLLIN;
        //LOG_DEBUG("Push socket:" << (int64_t)items[i].socket << " into polling set");
    }
         
    return 0;
}

int ZMQReceiverThread::do_tcp_listen(const std::vector<std::pair<std::string,uint16_t> >& tcp_addr_list)
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



int ZMQReceiverThread::do_auto_tcp_listen(uint16_t start_port, int port_count)
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

int ZMQReceiverThread::do_ipc_listen(const std::vector<std::string>& ipc_addr_list)
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

void ZMQReceiverThread::get_listen_addr(
        std::vector<std::pair<std::string,uint16_t> >& tcp_addr_list, 
        std::vector<std::string>& ipc_addr_list)
{
    tcp_addr_list = m_tcp_addr_list;
    ipc_addr_list = m_ipc_addr_list;
    
}

void ZMQReceiverThread::get_listen_addr(
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

void ZMQReceiverThread::output_stat()
{
    std::ofstream outfile ("receiver_time.txt");
    char buff[1204];
    for (int i=0; i<received_num; ++i)
    {
        sprintf(buff, "%ld\t%ld.%06ld\n",
            call_list[i], time_list[i].tv_sec, time_list[i].tv_usec);
            
        outfile << buff;
    }
    outfile.close();
    //exit(0);
}

void ZMQReceiverThread::handle_incoming_msg(RpcHead* rpc_head, IMsg* msg)
{
        LOG_DEBUG("Handle an incoming msg, RpcHead, msg_type:" << rpc_head->msg_type
            << ", call_id:" << rpc_head->call_id
            <<  ", transaction_id:" << rpc_head->transaction_id
            << ", responser_type:" << rpc_head->responser.service_type
            << ", invoker_type:" << rpc_head->invoker.service_type
            << ", biz_handler_id:" << rpc_head->biz_handler_id);
        
        IMsg* original_req = NULL;

        IStage* biz_stage = NULL;

        if (rpc_head->msg_type > 0)
        {
            //Req
            biz_stage = ServiceStageMap::instance()->get_stage(rpc_head->responser.service_type);
            if (biz_stage == NULL)
            {
                LOG_ERROR("Unsupported service_type:" << rpc_head->responser.service_type);
                delete rpc_head;
                msg->free();
                return;
            }
            LOG_DEBUG("Req target stage is:" << biz_stage->get_name());
        }
        else
        {
            //Res

            //To remove the call out record
            size_t target_call_out_record = rpc_head->call_id % m_call_out_record_list.size();
            int r= m_call_out_record_list[target_call_out_record]->remove(rpc_head->call_id, original_req);
            if (r != 0)
            {
                LOG_ERROR("call_id:" << rpc_head->call_id 
                    << ", does not exist in call_out_record."
                    << "It might time out.");
                return;
            }      

            biz_stage = StageMgr::instance()->get_stage(rpc_head->biz_handler_id);
            LOG_DEBUG("Res target stage is:" << biz_stage->get_name());

        }

        RpcEvent* event = new RpcEvent(msg, rpc_head);
        event->set_req(original_req);
        biz_stage->get_sink()->enqueue(event);
}
        
void ZMQReceiverThread::run()
{
    LOG_DEBUG("Begin...");    
    
    int rc = 0;
    m_lock.lock();
    while (1) 
    {
        // Wait for next request from client
        zmq_msg_t request;
        rc = zmq_poll(m_poll_items, m_zmq_socket_list.size(), -1);
        if (rc < 0)
        {
            //LOG_ERROR("zmq_poll error:" << zmq_strerror(errno));
            continue;            
        }
        
        for (size_t i=0; i<m_zmq_socket_list.size(); ++i)
        {
            if (m_poll_items[i].revents & ZMQ_POLLIN)
            {
                zmq_msg_init(&request);
                zmq_recv(m_poll_items[i].socket, &request, 0);
                

                size_t msg_size = zmq_msg_size(&request);
                
                if (msg_size < sizeof(TransHead))
                {
                    zmq_msg_close (&request);
                    continue;
                }
                
                if (msg_size > 5*1024*1024)
                {
                    LOG_WARN("msg is too big. size:" << msg_size
                        << ", limit: 5MB. Discarded!");
                    zmq_msg_close (&request);
                    continue;                        
                } 
                
                m_lock.unlock();
                TransHead trans_head;
                trans_head.ntoh_deserialize((uint8_t*)zmq_msg_data(&request));
                if (trans_head.rpc_head_len + trans_head.rpc_msg_len + sizeof(trans_head) != msg_size)
                {
                    LOG_ERROR("Received an invalid msg");
                    zmq_msg_close (&request);
                    m_lock.lock();
                    continue;                    
                }
                
                LOG_DEBUG("Received a msg, TransHead, version:" << trans_head.version
                    << ", rpc_head_len:" << trans_head.rpc_head_len
                    << ", rpc_msg_len:" << trans_head.rpc_msg_len
                    << ", call_id:" << trans_head.call_id);                

                RpcHead* rpc_head = new RpcHead();
                uint8_t* rpc_head_buff = (uint8_t*)zmq_msg_data(&request)+sizeof(TransHead);
                uint8_t* msg_buff = rpc_head_buff + sizeof(RpcHead);
                rpc_head->ntoh_deserialize(rpc_head_buff);
                
                LOG_DEBUG("Received a msg, RpcHead, msg_type:" << rpc_head->msg_type
                    << ", call_id:" << rpc_head->call_id 
                    << ",tranaction_id:" << rpc_head->transaction_id 
                    << ", invoker_type:" << rpc_head->invoker.service_type
                    << ", invoker.instance_id:" << rpc_head->invoker.instance_id
                    << ", responser_type:" << rpc_head->responser.service_type
                    << ", trans_param:" << rpc_head->trans_param);
                    
                IMsg* msg = m_msg_factory->deserialize_body(msg_buff,
                    trans_head.rpc_msg_len, rpc_head->msg_type);
                zmq_msg_close (&request);

                if (msg == NULL)
                {
                    LOG_ERROR("Failed to deserialize rpc msg, msg_type:" << rpc_head->msg_type);
                    delete rpc_head;
                    m_lock.lock();
                    continue;
                }                        
                
                handle_incoming_msg(rpc_head, msg);
                m_lock.lock();
            }
        }


    }
}

void ZMQReceiverThread::stop()
{
    //TODO: Send one-byte msg to the zmq socket
    //to make the loop in the run() exit
}
    
    
//ZMQReceiver
///////////////////////////        
  

int ZMQReceiver::init(void* param)
{
    int r = 0;
    
    r = m_recv_thread.init(param);
    if (r != 0)
    {
        LOG_FATAL("Failed to init trans recv thread");
    }
    
   
    return r; 
}

int ZMQReceiver::start()
{
    int r = 0;
    r = m_recv_thread.start();
    
    return r;
}

void ZMQReceiver::stop()
{
    return m_recv_thread.stop();
}


void ZMQReceiver::get_listen_addr(
        std::vector<std::pair<std::string,uint16_t> >& tcp_addr_list, 
        std::vector<std::string>& ipc_addr_list)
{
    m_recv_thread.get_listen_addr(tcp_addr_list, ipc_addr_list);
}

void ZMQReceiver::get_listen_addr(
    std::set<std::pair<std::string,uint16_t> >& tcp_addr_set, 
    std::set<std::string>& ipc_addr_set)
{
    m_recv_thread.get_listen_addr(tcp_addr_set, ipc_addr_set);
}



