
#ifndef ZONDA_COMMON_SERVICE_INSTANCE_ID_H_
#define ZONDA_COMMON_SERVICE_INSTANCE_ID_H_


#include <vector>
#include <iostream>
#include "util/Singleton.h"
#include "util/byte_order.h"
#include "util/string_ex.h"

namespace zonda
{
namespace common
{

using namespace std;
    
union ServiceInstanceId
{
private:
    int64_t value;
    struct
    {
        int32_t ip;
        uint16_t port;
        int16_t service_type;
    };
    
    uint8_t ba[8];
public:
    void set(int64_t id)
    {
        value = id;
    }
    void set(uint16_t service_type, const char* ip, uint16_t port)
    {
        struct sockaddr_in addr;
        
        // store this IP address in sa:
        inet_pton(AF_INET, ip, &(addr.sin_addr));
        this->ip = addr.sin_addr.s_addr;
        this->port = port;
        this->service_type = service_type;        
    }
    
    int set_by_str(const string& id_str)
    {
        size_t pos1 = id_str.find('.');
        if (pos1 == string::npos)
        {
            return -1;
        }
        
        uint16_t st = atoi(id_str.substr(0, pos1).c_str());
        size_t pos2 = id_str.rfind('.');
        if (pos2 == string::npos)
        {
            return -2;
        }
       
        uint16_t port_num = atoi(id_str.substr(pos2+1).c_str());
        //cout << "service_type:" << service_type
        //    << ", port:" << port
        //    << ", ip:" << id_str.substr(pos1+1, pos2-pos1-1) << endl;
        
        set(st, id_str.substr(pos1+1, pos2-pos1-1).c_str(), port_num);
        
        return 0; 
    }
    // require strlen(ip_str)>= 16
    void get_addr(char* ip, uint16_t& port)
    {
        struct sockaddr_in addr;
        addr.sin_addr.s_addr = this->ip;
        inet_ntop(AF_INET, &addr.sin_addr, ip, 16);
        port = this->port;
    }
    
    int64_t get_value()
    {
        return value;
    }
    
    string to_str()
    {
        char buff[200];
        char ip[20];
        uint16_t port;
        get_addr(ip, port);        
        sprintf(buff, "%d.%s.%d", service_type, ip, port);
        string id_str(buff);
        return id_str; 
    }
    
    void hton()
    {
        port = htons(port);
        service_type = htons(service_type);
    }
    
    void ntoh()
    {
        port = ntohs(port);
        service_type = ntohs(service_type);
    }
}; 

} //end namespace common
} //end namespace zonda
#endif //ZONDA_COMMON_SERVICE_INSTANCE_ID_H_

