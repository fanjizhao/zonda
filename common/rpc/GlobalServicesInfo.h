#ifndef ZONDA_COMMON_GLOBAL_SERVICE_INFO_H
#define ZONDA_COMMON_GLOBALSERVICEINFO_H

#include <vector>
#include <string>
#include <stdint.h>
#include <util/Singleton.h>

namespace zonda
{
namespace common
{

using namespace std;

class GlobalServicesInfoImpl
{
public:
    int set_service_count(uint16_t count);
    uint16_t get_service_count();
    int set_req_name(const char** req_name_list);
    int set_res_name(const char** res_name_list);
    int set_service_name(const char** service_name_list);
    const char* get_service_name(int16_t service_type);
    const char* get_msg_name(int msg_type);

private:
    uint16_t m_service_count;
    vector<string>  m_req_name_list;
    vector<string>  m_res_name_list;
    vector<string>  m_service_name_list;
};


typedef Singleton<GlobalServicesInfoImpl>    GlobalServicesInfo;



} //end namespace common
} //end namespace zonda

#endif //ZONDA_COMMON_GLOBALSERVICEINFO_H
