
#include "GlobalServicesInfo.h"
#include <iostream>
using namespace zonda::common;
    
int GlobalServicesInfoImpl::set_service_count(uint16_t count)
{
    m_service_count = count;
    string s("");
    
    for(size_t i = 0; i<count; ++i)
    {
        m_service_name_list.push_back(s);
    }
    
    
    for (size_t i=0; i<count*100U; ++i)
    {
        m_req_name_list.push_back(s);
        m_res_name_list.push_back(s);
    }
    
    return 0;
}

uint16_t GlobalServicesInfoImpl::get_service_count()
{
    return m_service_count;
}
int GlobalServicesInfoImpl::set_req_name(const char** req_name_list)
{
    for (int16_t i=0; i< m_service_count*100; ++i)
    {
        m_req_name_list[i] = req_name_list[i]? req_name_list[i]: "";
    } 
    return 0;
}

int GlobalServicesInfoImpl::set_res_name(const char** res_name_list)
{
    for (int16_t i=0; i< m_service_count*100; ++i)
    {
        m_res_name_list[i] = res_name_list[i]? res_name_list[i]:"";
    } 

    return 0;
}

int GlobalServicesInfoImpl::set_service_name(const char** service_name_list)
{
    for (int16_t i=0; i< m_service_count; ++i)
    {
        m_service_name_list[i] = service_name_list[i];
    } 
    
    return 0;
}
const char* GlobalServicesInfoImpl::get_service_name(int16_t service_type)
{
    if (service_type < 0 || service_type >= m_service_count ) return "UNKNOW";
    
    return m_service_name_list[service_type].c_str();
}

const char* GlobalServicesInfoImpl::get_msg_name(int msg_type)
{
    if (msg_type > 0 && msg_type < m_req_name_list.size())
    {
        return m_req_name_list[msg_type].c_str();
    }
    else if (msg_type < 0 && (0-msg_type) < m_res_name_list.size())
    {
        //using namespace std;
        //cout << "0-msg_type" << 0 - msg_type << endl;
        return m_res_name_list[0 - msg_type].c_str();
    }
    
    return "UNKNOW";
}





