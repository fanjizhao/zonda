#include <cassert>
#include <fstream>
#include <errno.h>
#include <utility>
#include <algorithm>
#include "RouteTable.h"
#include "log/Logger.h"
#include "util/string_ex.h"
#include "util/net_util.h"
#include "RouteStrategy.h"
#include "ServiceStageMap.h"
#include "ServiceGroupingTable.h"
#include "GlobalServicesInfo.h"
#include "MasterSlaveSwitchEvent.h"

   
using namespace zonda::common;

using namespace std;


void RouteTableImpl::ServiceItem::update_service_seq()
{
    size_t pos;
    pos = this->instance_name.rfind("-");
    if (pos != string::npos)
    {
        string s = this->instance_name.substr(pos+1);
        this->service_seq = strtoll(s.c_str(), NULL, 10); 
    }
    else
    {
        this->service_seq = -1;
    }    
}

void RouteTableImpl::ServiceItem::reset()
{
    tcp_addr_list.clear();
    ipc_addr_list.clear();
}

RouteTableImpl::RouteTableImpl()
{
    m_service_count = GlobalServicesInfo::instance()->get_service_count();
    m_service_list.resize(m_service_count);
    m_local_service_list.resize(m_service_count);
    m_local_host_service_list.resize(m_service_count);
    

    m_group_list.resize(m_service_count);
    for (size_t i=0;i<m_service_count; ++i)
    {
        
        m_group_list[i].resize(ServiceGroupingTable::MAX_GROUP_COUNT);
    }
}
 
int RouteTableImpl::load(const char* buff, int data_len)
{

    m_lock.lock();
    unordered_map<INSTANCE_ID_TYPE, ServiceItem>  new_service_map;
    vector< vector<ServiceItem> > new_service_list;
    vector< vector<ServiceItem> > new_local_service_list;
    vector< vector<ServiceItem> > new_local_host_service_list;
 
    new_service_list.resize(m_service_count);
    new_local_service_list.resize(m_service_count);
    new_local_host_service_list.resize(m_service_count);
 
    int r = get_local_ip_addr(m_local_ip_set);
    if (r != 0)
    {
        LOG_ERROR("Failed to get the local ip addr set");
        m_lock.unlock();
        return -1;
    }   
    
    unordered_set<std::string>::iterator it;
    for (it = m_local_ip_set.begin(); it != m_local_ip_set.end(); ++it)
    {
        LOG_DEBUG("local ip: " << *it);
    }
    
    std::string content(buff, data_len);
    std::vector<std::string> lines;
    split_str(content, lines, '\n');
    string current_service;
    ServiceItem item;
    for (size_t i=0; i<lines.size(); ++i)
    {
        current_service.clear();

        
        std::string line = trim(lines[i]);
        if (line.size() == 0 || line[0] == '#')
        {
            continue;
        }
        

        vector<string> tmp;
        split_str(line, tmp, '=', 1);
        
        size_t pos;
        pos = tmp[0].rfind("service_type");
        if (pos != string::npos)
        {
            //Find the begining of the service info
            item.instance_name = tmp[0].substr(0,pos-1);
            item.service_type = atoi(tmp[1].c_str());
            item.update_service_seq();
            LOG_DEBUG("instance_name = " << item.instance_name
                << ", service_seq=" << item.service_seq
                << ", service_type:" << item.service_type);
                
            continue;
        }
        

        //Find the instance_id
        pos = tmp[0].rfind("instance_id");
        if (pos != string::npos)
        {
            ServiceInstanceId id;
            if (id.set_by_str(tmp[1]) != 0)
            {
                LOG_ERROR("Failed to parse instance_id. Id:" << tmp[1]);
                m_lock.unlock();
                return -1;                
            }
            item.instance_id = id.get_value();
            LOG_DEBUG("instance_id:" << tmp[1].c_str()
                << ", digital form:"  << item.instance_id);
            continue;
        }
        
        //Find the tcp_addr
        pos = tmp[0].rfind("tcp_addr");
        if (pos != string::npos)
        {
            std::vector<std::string> tcp_addr_list;
            split_str(tmp[1], tcp_addr_list, ',');
            if (tcp_addr_list.size() <= 0)
            {
                LOG_ERROR("No tcp_addr for this service:" << item.instance_name);
            }
            std::pair<std::string, uint16_t> tcp_addr;
            for (size_t i=0; i<tcp_addr_list.size(); ++i)
            {
                str_to_tcp_addr(tcp_addr_list[i], tcp_addr);
                item.tcp_addr_list.push_back(tcp_addr);
                LOG_DEBUG(item.instance_name << ",ip:" 
                    << tcp_addr.first << ", port:" 
                    << tcp_addr.second);
            }
            
            //Find the group_id
            item.group_id = ServiceGroupingTable::instance()->get_group_id(item.service_type, item.tcp_addr_list[0].first);
            LOG_DEBUG("group_id:" << item.group_id);
            continue;            
        } 
        
        //Find the ipc_addr
        pos = tmp[0].rfind("ipc_addr");
        if (pos != string::npos)
        {
            split_str(tmp[1], item.ipc_addr_list, ',');           
            //LOG_DEBUG("ipc_addr:" << item.ipc_addr);
            continue;
        } 
        
        
        //Find the state
        pos = tmp[0].rfind("state");
        if (pos != string::npos)
        {           
            item.state = atoi(tmp[1].c_str());
            LOG_DEBUG("state:" << item.state);      
            
            if (update_service_map(item, new_service_map) != 0)
            {
                m_lock.unlock();
                return -1;
            }
            
            //Update m_serv_instance_map
            update_service_list(item, new_service_list);     
    
            //Update m_serv_addr_map
            //update_addr_map(item);             
            update_local_service_list(item, new_local_service_list);
            update_local_host_service_list(item, new_local_host_service_list);
            item.reset();              
        } 
        
        
 

    }
    
    if (new_service_list.size() > 0 && new_local_service_list.size() > 0
        && new_local_host_service_list.size() > 0)
    {
        new_service_list.swap(m_service_list);
        new_local_service_list.swap(m_local_service_list);
        new_local_host_service_list.swap(m_local_host_service_list);
        new_service_map.swap(m_service_map);
        update_group_list();
        LOG_INFO("RouteTable has been updated successfully!");
    }
    else
    {
        LOG_WARN("No valid content for RouteTable to load"
            << ". new_service_list.size: " << new_service_list.size()
            << ". new_local_service_list.size:" << new_local_service_list.size()
            << ". new_local_host_service_list:" << new_local_host_service_list.size()); 
    }
          
    m_lock.unlock();
    
    return 0;
}

int RouteTableImpl::update_service_map(const ServiceItem& item,
    unordered_map<INSTANCE_ID_TYPE, ServiceItem>& new_map)
{
    unordered_map<INSTANCE_ID_TYPE, ServiceItem>::iterator it;
    it = new_map.find(item.instance_id);
    if (it != new_map.end())
    {
        LOG_FATAL("Duplicated service item, instance_name:" 
            << item.instance_name << ", instance_id:"
            << item.instance_id);
        return -1;
    }
    
    new_map[item.instance_id] = item;
    
    return 0;
}
        
void RouteTableImpl::update_service_list(const ServiceItem& item, 
    vector< vector<ServiceItem> >& service_list)
{
    service_list[item.service_type].push_back(item);

    LOG_DEBUG("service_type:" << item.service_type
        << ", instance_id:" << item.instance_id
        << ", ip:" << item.tcp_addr_list[0].first
        << ", port:" << item.tcp_addr_list[0].second);    
}


void RouteTableImpl::update_local_service_list(const ServiceItem& item,
    vector< vector<ServiceItem> >& local_service_list)
{
    if ( !ServiceStageMap::instance()->is_local_service(item.instance_id))
    {
        LOG_DEBUG("instance_id:" << item.instance_id << " is not local service. "
            << "So don't put it into local_service");
        return;
    }
    
    local_service_list[item.service_type].push_back(item);    
    LOG_DEBUG(item.instance_name << " is local service"
        << ", instance_id:" << item.instance_id << ". "
        << "So put it into local_service");
}

void RouteTableImpl::update_local_host_service_list(const ServiceItem& item,
    vector< vector<ServiceItem> >& local_host_service_list)
{
    //A service instance could not listen on IPs which are on different host
    //, so we only need to check the first IP
    if (!is_local_ip(item.tcp_addr_list[0].first))
    {
        LOG_DEBUG("instance_id: " << item.instance_id
            << " is not local host service. ip:" 
            << item.tcp_addr_list[0].first);
        return;
    }    
    
    
    local_host_service_list[item.service_type].push_back(item);
    LOG_DEBUG(item.instance_name << " is local host service"
        << ", instance_id:" << item.instance_id);

}
 
void RouteTableImpl::update_group_list()
{
    //Get the old role for local service instances
    vector<int64_t> local_instance_list;
    ServiceStageMap::instance()->get_local_service_instance_list(local_instance_list);
    map<int64_t, int> old_role_map;

    unordered_map<INSTANCE_ID_TYPE, ServiceItem>::iterator it;    
    for (size_t i=0; i<local_instance_list.size(); ++i)
    {
        it = m_service_map.find(local_instance_list[i]);
        if (it == m_service_map.end())
        {
            LOG_FATAL("instance_id:" << local_instance_list[i]
                << " is not in m_service_map");
            continue;
        }
        
        
        int role = ServiceInstanceRole::UNKNOWN;
        uint16_t service_type = it->second.service_type;
        size_t group_id = (size_t)it->second.group_id;
        int pos = -1;

        for (size_t j=0; j<m_group_list[service_type][group_id].size(); ++j)
        {
            LOG_DEBUG("service_type:" << service_type
                << ", group_id:" << group_id
                << ", instance_id:" 
                << m_group_list[service_type][group_id][j].instance_id);  
                          
            if (m_group_list[service_type][group_id][j].instance_id == local_instance_list[i])
            {
                pos = j;
            }
        }

        if (pos == 0)
        {
            role = ServiceInstanceRole::MASTER;
        }
        else if (pos > 0)
        {
            role = ServiceInstanceRole::SLAVE;
        }

        old_role_map[it->second.instance_id] = role;
        LOG_DEBUG("service_type:" << service_type 
            << ", group_id:" << group_id
            << ", instance_id:" << it->second.instance_id
            << ", old role:" << ServiceInstanceRole::get_desc(role));
    }
    
    
    //Update the group list    
    for (size_t i=0; i<m_service_count; ++i)
    {
        for (size_t j=0; j<ServiceGroupingTable::MAX_GROUP_COUNT; ++j)
        {
            m_group_list[i][j].clear();
        }
    }

    for (it = m_service_map.begin(); it != m_service_map.end(); ++it)
    {
        m_group_list[it->second.service_type][it->second.group_id].push_back(it->second);
    }
    
    compare_by_sequence comp;
    for (size_t i=0; i<m_service_count; ++i)
    {
        for (size_t j=0; j<ServiceGroupingTable::MAX_GROUP_COUNT; ++j)
        {
            //After the sort, the first will be the master, the rest will be the slaves
            sort(m_group_list[i][j].begin(), m_group_list[i][j].end(), comp);
        }
    }
    
    //Compare the old role and the new role for local service instances
    //To throw notification if needed
    map<int64_t, int>::iterator old_it;
    int old_role;
    int new_role;
    uint16_t route_strategy = 0;    

    for (old_it = old_role_map.begin(); old_it != old_role_map.end(); ++old_it)
    {
        old_role = old_it->second;
        
        it = m_service_map.find(old_it->first);        
        
        uint16_t service_type = it->second.service_type;
        size_t group_id = (size_t)it->second.group_id;
        if (m_group_list[service_type][group_id][0].instance_id == old_it->first)
        {
            new_role = ServiceInstanceRole::MASTER;
        }
        else
        {
            new_role = ServiceInstanceRole::SLAVE;
        }

        RouteStrategy::instance()->get_route_strategy(service_type, route_strategy); 
        

        LOG_DEBUG("service_type:" << service_type 
            << ", route_strategy:" << route_strategy
            << ", group_id:" << group_id
            << ", instance_id:" << old_it->first
            << ", old role:" << ServiceInstanceRole::get_desc(old_role)
            << ", new  role:" << ServiceInstanceRole::get_desc(new_role));
            
        if ( old_role !=  new_role && route_strategy == RouteStrategy::MASTER_SLAVE)
        {
            IEventSink* sink = ServiceStageMap::instance()->get_stage(service_type)->get_sink();
            MasterSlaveSwitchEvent* event = new MasterSlaveSwitchEvent();
            event->service_type = service_type;
            event->old_role = old_role;
            event->new_role = new_role;
            sink->enqueue(event);
        }
    }    
}       

bool RouteTableImpl::is_local_ip(const std::string& ip)
{
    if (m_local_ip_set.find(ip) != m_local_ip_set.end()) 
    {
        return true;
    }
    
    return false;
}




void RouteTableImpl::update_ipc_map(const ServiceItem& item)
{
}

int RouteTableImpl::load(const char* file_path)
{
    std::ifstream config_file;
	config_file.open(file_path);
	if (!config_file.is_open())
	{
		LOG_ERROR("open file error, filename:" << file_path
			<< ", error:" << strerror(errno));
		return -1;
	}
	
    std::stringstream buffer;
    buffer << config_file.rdbuf();
    
    config_file.close();
    return load(buffer.str().c_str(), buffer.str().size());    
}

int RouteTableImpl::choose_round_robin(const ServiceAddr& responser, 
    std::string& host, uint16_t &port)
{
    LOG_DEBUG("Service_type: " << responser.service_type);
    if (responser.service_type >= m_service_count)
    {
        LOG_ERROR("Invalid service_type:" << responser.service_type
            << ", m_service_count:" << m_service_count);
        return -1;
    }
    
    if (m_service_list[responser.service_type].empty())
    {
        LOG_ERROR("service_type:" << responser.service_type
            << ", no instance");
        return -1;        
    }

    
    //NOTE: Currently, we always choose the first first listened tcp addr
    m_service_round_robin[responser.service_type] = (m_service_round_robin[responser.service_type]+1)%m_service_list[responser.service_type].size();
    int next = m_service_round_robin[responser.service_type];
    host = m_service_list[responser.service_type][next].tcp_addr_list[0].first;
    port = m_service_list[responser.service_type][next].tcp_addr_list[0].second;
    
    LOG_DEBUG("serviceType:" << responser.service_type
        << ", next:" << next
        << ", " << host << ":" << port);
    return 0; 
}

int RouteTableImpl::choose_directly(const ServiceAddr& responser, 
    std::string& host, uint16_t &port)
{
   
    unordered_map<INSTANCE_ID_TYPE, ServiceItem>::iterator it;
    
    it = m_service_map.find(responser.instance_id);
    
    if (it == m_service_map.end())
    {
        LOG_ERROR("Not found service_type:" << responser.service_type
            << ", instance_id:" << responser.instance_id);
        return -1;
    }
    
    //NOTE: This strategy is just a sample, so we always choose the first
    //tcp addr if this instance bind on multi tcp addr
    host = it->second.tcp_addr_list[0].first;
    port = it->second.tcp_addr_list[0].second;
    
    return 0;     
}

int RouteTableImpl::choose_localhost(const ServiceAddr& responser, 
    std::string& host, uint16_t &port)
{

    if (responser.service_type >= m_service_count)
    {
        LOG_ERROR("Invalid service_type:" << responser.service_type);
        return -1;
    }


    if (m_local_host_service_list[responser.service_type].empty())
    {
        LOG_ERROR("Not found such service_type:" << responser.service_type
            << " on local host services");
        return -1;
    }
    
   
    //NOTE: This strategy is just a sample, so we always choose the first
    //tcp addr if this instance bind on multi tcp addr
    host = m_local_host_service_list[responser.service_type][0].tcp_addr_list[0].first;
    port = m_local_host_service_list[responser.service_type][0].tcp_addr_list[0].second;
    
    return 0;      
}


int RouteTableImpl::choose_master(const ServiceAddr& responser, 
        std::string& host, uint16_t &port)
{
    if (responser.service_type >= m_service_count)
    {
        LOG_ERROR("Invalid service_type:" << responser.service_type);
        return -1;
    }
    
    if (responser.group_id >= ServiceGroupingTable::MAX_GROUP_COUNT)
    {
        LOG_ERROR("Invalid group_id:" << responser.group_id);
        return -1;        
    }
    
    size_t size = m_group_list[responser.service_type][responser.group_id].size();
    if (size == 0)
    {
        LOG_ERROR("No found valid item. service_type:" << responser.service_type
            << ", group_id:" << responser.group_id);
        return -1;         
    }
    vector<ServiceItem>::iterator it;
    it = m_group_list[responser.service_type][responser.group_id].begin();
    if (responser.role == 0)
    {
        //Want master
    }
    else
    {
        //NOTE: Currently we only choose the first slave as the target.
        //If there is only one instance in the group, namely we choose the master
        //as the slave
        if (size == 1)
        {
        }
        else
        {
            ++it;
        }
    }   

    //NOTE: This strategy is just a sample, so we always choose the first
    //tcp addr if this instance bind on multi tcp addr
    host = it->tcp_addr_list[0].first;
    port = it->tcp_addr_list[0].second;    

    return 0;
}   


int RouteTableImpl::get_real_addr(
    const ServiceAddr& responser, 
    std::string& host, 
    uint16_t &port)
{
    LOG_DEBUG("Begin..., service_type:" << responser.service_type);
    if (responser.instance_id != 0)
    {
        ServiceInstanceId id;
        id.set(responser.instance_id);
        char ip[20];
        id.get_addr(ip, port);
        host = ip;
        LOG_DEBUG("Got addr from instance_id:" << id.get_value()
            << ", " << ip << ":" << port); 
        return 0;
    }

    m_lock.lock();
    
    uint16_t route_strategy = 0;
    int r = 0;
    r = RouteStrategy::instance()->get_route_strategy(responser.service_type, route_strategy);
    if (r != 0)
    {
        m_lock.unlock();
        LOG_ERROR("can't find route_strtegy for service_type:" << responser.service_type);
        return -1;
    }
    
    LOG_DEBUG("service_type:" << responser.service_type
        << ", route_strategy:" 
        << RouteStrategy::get_strategy_str(route_strategy) );    
    switch(route_strategy)
    {
        case zonda::common::RouteStrategy::DIRECT:
            r = choose_directly(responser, host, port);
            break;
        case zonda::common::RouteStrategy::LOCALHOST:
            r = choose_localhost(responser, host, port);
            break;
        case zonda::common::RouteStrategy::ROUND_ROBIN:
            r = choose_round_robin(responser, host, port);
            break;
        case zonda::common::RouteStrategy::MASTER_SLAVE:
            r = choose_master(responser, host, port);
            break;
        default:
            m_lock.unlock();
            LOG_ERROR("Unsupported route strategy: " << route_strategy);
            return -1;
    }

    m_lock.unlock();
    if (r != 0)
    {
        LOG_ERROR("Failed to get real addr for service_type:"
            << responser.service_type << 
            ".It may not exist in the route table");
        return r;
    }
    
    LOG_DEBUG("Got the real addr, service_type:" << responser.service_type
        << ", instance_id:" << responser.instance_id
        << ", return host:" << host
        << ", port:" << port);

    return r;
}

void RouteTableImpl::set_local_listen_addr(
    std::vector<std::pair<std::string,uint16_t> >& tcp_addr_list, 
    std::vector<std::string>& ipc_addr_list)
{
    m_lock.lock();
    m_local_tcp_addr_list = tcp_addr_list;
    m_local_ipc_addr_list = ipc_addr_list;
    m_lock.unlock();
}
            
 

int RouteTableImpl::get_service_item_list(uint16_t service_type, vector<ServiceItem>& item_list)
{
    LOG_DEBUG("Service_type: " << service_type);
    m_lock.lock();

    if (service_type >= m_service_list.size())
    {
        LOG_FATAL("Not found service_type:" << service_type << " in m_service_list");
        m_lock.unlock();
        return -1;
    }
    
  
    item_list =  m_service_list[service_type];  
    m_lock.unlock();
    return 0;
}
      


