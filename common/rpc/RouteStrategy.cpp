#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "RouteStrategy.h"
#include "util/string_ex.h"
#include "log/Logger.h"
#include "GlobalServicesInfo.h"

using namespace zonda::common;

const char* RouteStrategyImpl::m_strategy_str[] = {
    "UNSUPPORTED",
    "DIRECT",
    "MODULO",
    "Consistent Hash",
    "ROUND_ROBIN",
    "LOCALHOST",
    "MASTER_SLAVE"
};
    

int RouteStrategyImpl::load(const char* buff, int buff_len)
{
    LOG_DEBUG("buff_len=" << buff_len << ", buff=" << buff);
    std::string content(buff, buff_len);
    std::vector<std::string> lines;
    split_str(content, lines, '\n');
    vector<uint16_t> new_vector;
    uint16_t service_count = GlobalServicesInfo::instance()->get_service_count();
    new_vector.resize(service_count, 0);
    std::string current_service;
    size_t valid_num = 0;

    std::vector<std::string> tmp;
    uint16_t service_type;
    uint16_t strategy;
    for (size_t i=0; i<lines.size(); ++i)
    {
        tmp.clear();
        std::string line = trim(lines[i]);
        if (line.size() == 0 || line[0] == '#')
        {
            continue;
        }
        split_str(lines[i], tmp, '=');
        if (tmp.size() != 2)
        {
            LOG_WARN("Skip invalid line in route_strategy data. line: "
                << lines[i] << ", tmp.size=" << tmp.size());
            continue;
        }
        
        service_type = atoi(tmp[0].c_str());
        if (service_type >= service_count)
        {
            LOG_FATAL("Invalid service_type:" << service_type
                << ". service_count:" << service_count);
            return -1;
        }
        strategy = atoi(tmp[1].c_str());
        if (strategy >= MAX_STRATEGY)
        {
            LOG_FATAL("Invalid strategy:" << strategy);
            return -1;
        }
        
        LOG_DEBUG(GlobalServicesInfo::instance()->get_service_name(service_type) << " = " 
            << RouteStrategy::get_strategy_str(strategy));
        new_vector[service_type] = strategy;
        ++valid_num;       
    }
    
    if (valid_num == 0)
    {
        LOG_ERROR("Not found valid data, so give up the update");
        return -1;        
    }

    m_lock.lock();
    m_vector.swap(new_vector);
    m_lock.unlock();
    LOG_INFO("RouteStrategy updated successfully !");
    return 0;  
}

int RouteStrategyImpl::get_route_strategy(uint16_t service_type, uint16_t& strategy)
{
    if (service_type == 0) //For client
    {
        strategy = DIRECT;
        return 0;
    }
    
    m_lock.lock();
    
    if (service_type >= m_vector.size())
    {
        LOG_ERROR("Failed to get route strategy for service:" << service_type);
        return -1;
    }
    
    strategy = m_vector[service_type];
    m_lock.unlock(); 
    return 0;       
}

const char* RouteStrategyImpl::get_strategy_str(uint16_t strategy)
{
    if (strategy > MAX_STRATEGY)
    {
        return "UNKNOW_STRATEGY";
    }
    
    return m_strategy_str[strategy];
}


