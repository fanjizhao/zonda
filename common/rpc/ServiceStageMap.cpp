

#include <cassert>
#include <cstring>
#include <algorithm>
#include "ServiceStageMap.h"
#include "config/Config.h"
#include "ServiceAddr.h"
#include "RouteStrategy.h"
#include "GlobalServicesInfo.h"
#include "log/Logger.h"


using namespace zonda::common;

ServiceStageMapImpl::ServiceStageMapImpl()
{

}



int ServiceStageMapImpl::set_stage_service(IStage* request_stage,
    uint16_t service_type,
    vector<pair<string, uint16_t> >& tcp_addr_list,
    vector<string>& ipc_addr_list)
{
    
    if (service_type == 0)
    {
        //Don't need to register for CLIENT 
        return 0;    
    }
    
    if (tcp_addr_list.size() == 0)
    {
        LOG_FATAL("Failed to set_stage_service, tcp_addr_list is empty");
        return -1;
    }
    
    
    m_lock.lock();
    unordered_map<uint16_t, set<pair<string, uint16_t> > >::iterator it;
    it = m_service_tcp_map.find(service_type);
    if (it != m_service_tcp_map.end())
    {
        LOG_ERROR("service: " << GlobalServicesInfo::instance()->get_service_name(service_type) 
            << " can't be binded more than one times");
        m_lock.unlock();
        return -1;
    }

    m_service_stage_map[service_type] = request_stage;

    set<pair<string, uint16_t> > tcp_addr_set;
    for (size_t i=0; i< tcp_addr_list.size(); ++i)
    {
        tcp_addr_set.insert(tcp_addr_list[i]);
    }
    m_service_tcp_map[service_type] = tcp_addr_set;
    
    unordered_set<string> ipc_addr_set;
    for (size_t i=0; i< ipc_addr_list.size(); ++i)
    {
        ipc_addr_set.insert(ipc_addr_list[i]);
    }
    m_service_ipc_map[service_type] = ipc_addr_set;
    
    int r = send_route_info(service_type, tcp_addr_list, ipc_addr_list);
    m_lock.unlock();
    return r;    
}
        
    
IStage* ServiceStageMapImpl::get_stage(uint16_t service_type)
{
    m_lock.lock();
    unordered_map<uint16_t, IStage*>::iterator it;
    it = m_service_stage_map.find(service_type);
    if (it != m_service_stage_map.end())
    {
        m_lock.unlock();
        return it->second;
    }
    m_lock.unlock();
    return NULL;
}

int ServiceStageMapImpl::send_route_info(
    uint16_t     service_type,
    vector<pair<string, uint16_t> >& tcp_addr_list,
    vector<string>& ipc_addr_list)
{
    
    sort(tcp_addr_list.begin(), tcp_addr_list.end());
    sort(ipc_addr_list.begin(), ipc_addr_list.end());

    Config *config = Config::instance();
    char service_instance_name[1024] = "";
    sprintf(service_instance_name, "%s-%s-%d-", GlobalServicesInfo::instance()->get_service_name(service_type), 
        tcp_addr_list[0].first.c_str(), getpid());
    char path[1024];
    strcpy(path, Config::ZONDA_RUNTIME_PATH);
    strcat(path, "/");
    strcat(path, service_instance_name);
    
    LOG_DEBUG("To check if \"" << path << "\" exists"); 
    
    int r = 0;
   
    
    string tcp_addr_str;
    string ipc_addr_str;
    char buff[1024];
    for (size_t i=0; i<tcp_addr_list.size(); ++i)
    {
        sprintf(buff, "%s:%d", tcp_addr_list[i].first.c_str(), tcp_addr_list[i].second);
        if (i != 0)
        {
            tcp_addr_str += ",";
        }
        tcp_addr_str += buff; 
    }    

    for (size_t i=0; i<ipc_addr_list.size(); ++i)
    {
        if (i != 0)
        {
            ipc_addr_str += ",";

        }
        ipc_addr_str += ipc_addr_list[i]; 
    }   
    
 
    

    
    stringstream route_ss;
    ServiceInstanceId id;
    id.set(service_type, tcp_addr_list[0].first.c_str(), tcp_addr_list[0].second);
    m_service_type_instance_id_map[service_type] = id.get_value();
    LOG_DEBUG("instance_id:" << id.to_str()
        << ", digital form:" << id.get_value());
    route_ss << service_instance_name << "service_type=" << service_type << "\n"
        << service_instance_name << "instance_id=" << id.to_str() << "\n"
        << service_instance_name << "tcp_addr=" << tcp_addr_str << "\n"
        << service_instance_name << "ipc_addr= " << ipc_addr_str << "\n"
        << service_instance_name << "state=" << ServiceState::INS << "\n";
    
    LOG_DEBUG("\n" << route_ss.str());

    LOG_INFO("To create " << path);
    r = config->create_path(path, route_ss.str().c_str(), 
        route_ss.str().size(), Config::NODE_EPHEMERAL|Config::NODE_SEQUENCE);
    if (r != 0)
    {
        LOG_FATAL("Failed to create " << path);
        return -1;
    }
        
    LOG_INFO("Successfully created " << path);
    return 0;
}

bool ServiceStageMapImpl::is_local_service(int64_t instance_id)
{
    m_lock.lock();
    if (m_service_type_instance_id_map.find(instance_id) == m_service_type_instance_id_map.end())
    {
        m_lock.unlock();
        return false;
    }
    m_lock.unlock();
    return true;
}

int ServiceStageMapImpl::set_local_listen_addr(vector<pair<string, uint16_t> >& tcp_addr_list)
{
    if (tcp_addr_list.size() == 0)
    {
        return -1;
    }
    
    sort(tcp_addr_list.begin(), tcp_addr_list.end());
    
    m_lock.lock();
    //NOTE: Currently we only use it to produce the instance id for CLIENT
    //So don't save it
    ServiceInstanceId id;
    id.set(0, tcp_addr_list[0].first.c_str(), tcp_addr_list[0].second);
    m_service_type_instance_id_map[0] = id.get_value();
    m_lock.unlock();
    
    return 0; 
}   

int ServiceStageMapImpl::get_self_instance_id(uint16_t service_type, int64_t& instance_id)
{
    m_lock.lock();
   
    unordered_map<uint16_t, int64_t>::iterator it;
    it = m_service_type_instance_id_map.find(service_type);
    if (it != m_service_type_instance_id_map.end())
    {
        instance_id = it->second;
        m_lock.unlock();
        return 0;        
    }

    LOG_ERROR("Not found service_type:" << service_type << "in local map");
    m_lock.unlock();
    return -1;
}

void ServiceStageMapImpl::get_local_service_instance_list(
    vector<int64_t>& instance_list, bool with_client_id)
{
    unordered_map<uint16_t, int64_t>::iterator it;
    for (it = m_service_type_instance_id_map.begin();
        it != m_service_type_instance_id_map.end();
        ++it)
    {
        if (with_client_id == false && it->first == 0)
        {
            continue;
        }
        instance_list.push_back(it->second);
    }
}

