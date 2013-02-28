#include "ServiceGroupingTable.h"
#include "log/Logger.h"
#include "util/string_ex.h"

using namespace zonda::common;

uint16_t ServiceGroupingTableImpl::MAX_GROUP_COUNT = 256;
  
int ServiceGroupingTableImpl::load(const char* data, int data_len)
{
    unordered_map<SERVICE_TYPE, unordered_map<string, GROUP_ID_TYPE> > new_map;
    unordered_map<SERVICE_TYPE, unordered_map<string, GROUP_ID_TYPE> >::iterator it;
    unordered_map<string, GROUP_ID_TYPE>::iterator it2;

    std::string content(data, data_len);
    std::vector<std::string> lines;
    split_str(content, lines, '\n');
    
    for (size_t i=0; i<lines.size(); ++i)
    {
        if (lines[i][0] == '#')
        {
            //skip comments
            continue;
        }
        std::vector<std::string> fields;
        //service_type:ip:group_id
        split_str(lines[i], fields, ':');
        if (fields.size() != 3)
        {
            LOG_WARN("Invalid line for service grouping table,[" << lines[i] << "]");
            continue;
        }
        
        uint16_t service_type = atoi(fields[0].c_str());
        uint16_t group_id = atoi(fields[2].c_str());
        LOG_DEBUG("service_type:" << service_type
            << ", ip:" << fields[1]
            << ", group_id:" << group_id);
        if (group_id >= MAX_GROUP_COUNT)
        {
            LOG_FATAL("Invalid group_id: " << group_id
                << ", must < 256");
            return -1; 
        }
            
        it = new_map.find(service_type);
        if (it == new_map.end())
        {
            unordered_map<string, GROUP_ID_TYPE> tmp_map;
            tmp_map[fields[1]] = group_id;
            new_map[service_type] = tmp_map;
            continue;           
        }
        
        it2 = it->second.find(fields[1]);
        if (it2 != it->second.end())
        {
            LOG_ERROR("Failed to find same service_type:ip, different group_id."
                << "service_type:" << service_type
                << ", ip:" << fields[1]
                << ", group_id:" << group_id);
            return -1;
        }
        
        (it->second)[fields[1]] = group_id;
    }
    
    m_lock.lock();
    m_map.swap(new_map);
    m_lock.unlock();
    
    return 0;
}


uint16_t ServiceGroupingTableImpl::get_group_id(uint16_t service_type, const std::string& ip)
{
    m_lock.lock();
    unordered_map<SERVICE_TYPE, unordered_map<string, GROUP_ID_TYPE> >::iterator it;
    it = m_map.find(service_type);
    if (it == m_map.end())
    {
        m_lock.unlock();
        LOG_DEBUG("Not found service_type:" << service_type);
        return 0;
    }
    
    unordered_map<string, GROUP_ID_TYPE>::iterator it2;
    
    it2 = it->second.find(ip);
    if (it2 == it->second.end())
    {
        m_lock.unlock();
        LOG_DEBUG("Not found service_type:" << service_type
            << ", ip:" << ip);
        return 0;
    }
    m_lock.unlock();
    
    LOG_DEBUG("Found service_type:" << service_type
        << ", ip:" << ip 
        << ", group_id:" << it2->second);
    return it2->second;
}



