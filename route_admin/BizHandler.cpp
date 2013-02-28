#include <vector>
#include <algorithm>
#include <set>
#include "util/net_util.h"
#include "config/Config.h"
#include "BizHandler.h"
#include "log/Logger.h"

using namespace std;
using namespace zonda::common;

BizHandler::BizHandler():
    m_my_stage(NULL), m_my_sink(NULL), m_master(false)
{
    m_my_seq = 0;
}

int BizHandler::init(zonda::common::IStage* stage, void* param)
{
    m_my_stage = stage;
    m_my_sink = stage->get_sink();
    
    string buff;
    int r = 0;
    Config *config = Config::instance();
    /*
    r = config->get_value(Config::ZONDA_ROUTE_ADMIN_PATH, buff, m_my_sink);
    if (r != 0)
    {
        LOG_FATAL("Failed to get the data of zonda_admin_path");
        return -1;
    }
    
    m_route_admin_count = atoi(buff.c_str());
    if (m_route_admin_count < 1 || m_route_admin_count > 10)
    {
        LOG_FATAL("Invalid route_admin_count: " << m_route_admin_count
            << ". It should be in the range of 1~10");
        return 0;
    }*/
    
    vector<string> ip_list;
    r = get_local_ip_addr(ip_list, true);
    if (r != 0)
    {
        LOG_FATAL("Failed to get local ip addr list");
        return -1;
    }
    
    if (ip_list.size() <= 0)
    {
        LOG_FATAL("No valid local ip addr");
        return -1;
    }
    
    m_my_ip = ip_list[0];
    
    //We only use the first non-loopback ip addr as our ip
    buff = Config::ZONDA_ROUTE_ADMIN_PATH;
    buff += "/";
    buff += m_my_ip;
    buff += "_";
    
    LOG_DEBUG("To create path " << buff);    
    r = config->create_path(buff.c_str(), NULL, 0, Config::NODE_EPHEMERAL|Config::NODE_SEQUENCE);
    if (r != 0)
    {
        LOG_FATAL("Failed to create path: " << buff);
        return -1;
    }
    
    
    r = config->get_children(Config::ZONDA_ROUTE_ADMIN_PATH, m_route_admin_list, m_my_sink);
    if (r != 0)
    {
        LOG_FATAL("Failed to get children of " << Config::ZONDA_ROUTE_ADMIN_PATH);
        return -1;
    }
        
    for (size_t i=0; i<m_route_admin_list.size(); ++i)
    {
        LOG_DEBUG("Found route_admin instance: " << m_route_admin_list[i]);
    }
    
    /*
    if (m_route_admin_list.size() > m_route_admin_count)
    {
        LOG_FATAL("Real route admin instance count is greater than route_admin_count"
            << "real:" << m_route_admin_list.size() 
            << ", route_admin_count:" << m_route_admin_count);
        return -1;
    }*/
    
  
    
    int64_t smallest_seq = 100000000000000;
    for (size_t i=0; i<m_route_admin_list.size(); ++i)
    {
        size_t pos = string::npos;
        int64_t s = get_path_seq(m_route_admin_list[i]);
        if (s < 0)
        {
            LOG_FATAL("Failed to get seq of" << m_route_admin_list[i]);
            return -1;            
        }
        if (s <  smallest_seq)
        {
            smallest_seq = s;
        }
        
        if (m_my_seq <= 0)
        {
            pos = m_route_admin_list[i].find(m_my_ip);
            if (pos != string::npos)
            {
                //This is my node
                m_my_seq = s;
                LOG_INFO("my path is " << m_route_admin_list[i]
                    << ", my seq=" << m_my_seq);
            }            
        }

    }
    
    
    vector<string> children_list;
    LOG_INFO("To get children of " << Config::ZONDA_RUNTIME_PATH);
    LOG_DEBUG("biz_stage sink=" << m_my_sink);
    r = config->get_children(Config::ZONDA_RUNTIME_PATH, children_list, m_my_sink);
    if (r != 0)
    {
        LOG_FATAL("Failed to get children of " << Config::ZONDA_RUNTIME_PATH);
        return -1;
    }
    
    LOG_INFO(Config::ZONDA_RUNTIME_PATH << " has " << children_list.size() << " children");
    r = get_services_data(children_list);    
    if (r != 0)
    {
        LOG_FATAL("Failed to call get_services_data");
        return r;
    }
    
    if (m_my_seq !=  smallest_seq)
    {   
        LOG_INFO("I'm not the master. smallest seq:" << smallest_seq
            << ", m_my_seq:" << m_my_seq);
        return 0;
    }
    else
    {
        LOG_INFO("I'm the master. smallest seq:" << smallest_seq
            << ", m_my_seq:" << m_my_seq);        
    }

    m_master = true;
    r = update_route_table();
    return r; 
}

int64_t BizHandler::get_path_seq(const string& path)
{
    size_t pos;
    pos = path.rfind("_");
    if (pos != string::npos)
    {
        string s = path.substr(pos+1);
        return strtoll(s.c_str(), NULL, 10);
    }
    else
    {
        return -1;
    }
}

int BizHandler::get_services_data(vector<string>& children_list)
{
    string child_data;
    string child_path;
    int r = 0;
    Config* config = Config::instance();
    for (size_t i=0; i<children_list.size(); ++i)
    {
        child_path.clear();
        child_path += Config::ZONDA_RUNTIME_PATH;
        child_path += "/";
        child_path += children_list[i];
        r = config->get_value(child_path,
            child_data, m_my_sink);
        if (r != 0)
        {
            LOG_FATAL("Failed to get data of " << child_path);
            return -1;
        }
        
        LOG_DEBUG(child_path << ", data=\n" << child_data);
        m_service_map[children_list[i]] = child_data;
    }
    
    return 0;
}
int BizHandler::handle_event(const zonda::common::IEvent* event)
{
    int event_type = event->get_type();
    switch (event_type)
    {
        case EventType::CONFIG_CHANGE_EVENT:
            handle_conf_change_event((ConfigChangeEvent*)event);
            return 0;
        default:
            LOG_WARN("Unknow event type: " << event_type);
            return 0;
    }
    
    return 0;
}

int BizHandler::handle_conf_change_event(zonda::common::ConfigChangeEvent* event)
{

    LOG_INFO("Received ConfChangeEvent, change_type:" << event->m_change_type);
    if (event->m_change_type == ConfigChangeEvent::CHILD)
    {
        if (event->m_path.find(Config::ZONDA_ROUTE_ADMIN_PATH) != string::npos)
        {
            return handle_route_admin_change(event);
        }
        
        if (event->m_path.find(Config::ZONDA_RUNTIME_PATH) != string::npos)
        {
            return handle_service_change(event);
        }
    }
    else if (event->m_change_type == ConfigChangeEvent::CHANGED)
    {
        /*
        if (event->m_path.find(Config::ZONDA_ROUTE_ADMIN_PATH) != string::npos)
        {
            return handle_route_admin_count_change(event);
        }*/
        
        if (event->m_path.find(Config::ZONDA_RUNTIME_PATH) != string::npos)
        {
            return handle_route_info_change(event);
        }
    }
    
    return 0;
    
}

int BizHandler::handle_route_admin_change(zonda::common::ConfigChangeEvent* event)
{
    LOG_INFO("route admin change");
    
    //If I am the master, I don't need to do anything.
    //If I am the slave, to check if I need to be the new master
    
    if (m_master)
    {
        LOG_INFO("Some newer route_admin died, we need to do nothing");
        return 0;
    }
    
    int64_t child_seq = 0;
    for (size_t i=0; i<event->m_children.size(); ++i)
    {
        child_seq = get_path_seq(event->m_children[i]);
        if (child_seq < m_my_seq)
        {
            LOG_INFO(event->m_children[i] 
                << " became the smallest, so it's the new master"
                << ", I'm still the slave. my_seq:" << m_my_seq);
            return 0;
        }
    }
    
    LOG_INFO("I'm the new master now!!!");
    m_master = true;
    
    return 0;
}

int BizHandler::handle_service_change(zonda::common::ConfigChangeEvent* event)
{
    //Some services died or born.
    size_t new_service_count = event->m_children.size();
    if (new_service_count > m_service_map.size())
    {
        //Some services born
        vector<string> new_services;
        for (size_t i=0; i<new_service_count; ++i)
        {
            if (m_service_map.find(event->m_children[i]) == m_service_map.end())
            {
                new_services.push_back(event->m_children[i]);
            }
        }
        
        //Now new_services only contains the new born service
        //To get these new born services' data
        int r = 0;
        r = get_services_data(new_services);       
        if (r != 0)
        {
            LOG_FATAL("Failed to call get_services_data for new born services");
            return -1;
        }

    }
    else if (new_service_count < m_service_map.size())
    {
        //Some services died
        set<string> new_set;
        for (size_t i=0; i<event->m_children.size(); ++i)
        {
            LOG_DEBUG("insert " << event->m_children[i] 
                << " into new_set");
            new_set.insert(event->m_children[i]);
        }
        
        unordered_map<string, string>::iterator it;
        for (it = m_service_map.begin(); it != m_service_map.end(); ++it)
        {
            if (new_set.find(it->first) == new_set.end())
            {
                LOG_INFO(it->first << " has been down!!!");
                m_service_map.erase(it);
            }
        }       
        
    }
    
            
    return update_route_table();
}

int BizHandler::handle_route_admin_count_change(zonda::common::ConfigChangeEvent* event)
{
    //TODO
    return 0;
}

int BizHandler::handle_route_info_change(zonda::common::ConfigChangeEvent* event)
{
    LOG_INFO(event->m_path << " data has changed");
    size_t pos = event->m_path.rfind("/");
    if (pos == string::npos)
    {
        LOG_ERROR(event->m_path << " is invalid");
        return 0;
    }
    
    string service;
    service = event->m_path.substr(pos + 1);
    LOG_DEBUG("changed service is " << service);
    Config* config = Config::instance();
    unordered_map<string, string>::iterator it;
    if (it == m_service_map.end())
    {
        LOG_ERROR("Should not go here");
        return 0;
    }
    
    m_service_map[service] = event->m_value;
    
    return update_route_table();
    
}

int BizHandler::update_route_table()
{
    
    string route_table;
    unordered_map<string, string>::iterator it;
    for (it = m_service_map.begin(); 
        it != m_service_map.end(); ++it)
    {
        route_table += it->second;
        route_table += "\n";
    }
    
   
    int r = 0;
    Config* config = Config::instance();
    r = config->set_value(Config::ZONDA_ROUTE_TABLE_PATH, route_table);
    
    if (r != 0)
    {
        LOG_FATAL("Failed to set the route_table's data");
        return -1;
    }
    
    return 0;
}

