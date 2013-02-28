
#include <cstring>
#include "net_util.h"

namespace zonda
{
namespace common
{
int get_local_ip_addr(std::vector<std::string> & ip_list, bool without_lo)
{
    int s;
    struct ifconf ifconf;
    struct ifreq ifr[50];
    int ifs;
    int i;
    
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) 
    {
        perror("socket");
        return -1;
    }
    
    ifconf.ifc_buf = (char *) ifr;
    ifconf.ifc_len = sizeof ifr;
    
    if (ioctl(s, SIOCGIFCONF, &ifconf) == -1) 
    {
        perror("ioctl");
        return -1;
    }
    
    ifs = ifconf.ifc_len / sizeof(ifr[0]);
    //printf("interfaces = %d:\n", ifs);
    for (i = 0; i < ifs; i++) 
    {
        if (without_lo)
        {
            if (strncmp(ifr[i].ifr_name, "lo", 2) == 0)
            {
                continue;
            }
        }
        char ip[INET_ADDRSTRLEN];
        struct sockaddr_in *s_in = (struct sockaddr_in *) &ifr[i].ifr_addr;
        
        if (!inet_ntop(AF_INET, &s_in->sin_addr, ip, sizeof(ip)))
        {
            //perror("inet_ntop");
            return -1;
        }
    
        //printf("%s - %s\n", ifr[i].ifr_name, ip);
        ip_list.push_back(ip);
    }
    
    close(s);
    
    return 0;
    
}

int get_local_ip_addr(std::set<std::string> & ip_set, bool without_lo)
{
    int s;
    struct ifconf ifconf;
    struct ifreq ifr[50];
    int ifs;
    int i;
    
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) 
    {
        perror("socket");
        return -1;
    }
    
    ifconf.ifc_buf = (char *) ifr;
    ifconf.ifc_len = sizeof ifr;
    
    if (ioctl(s, SIOCGIFCONF, &ifconf) == -1) 
    {
        perror("ioctl");
        return -1;
    }
    
    ifs = ifconf.ifc_len / sizeof(ifr[0]);
    //printf("interfaces = %d:\n", ifs);
    for (i = 0; i < ifs; i++) 
    {
        if (without_lo)
        {
            if (strncmp(ifr[i].ifr_name, "lo", 2) == 0)
            {
                continue;
            }
        }
                
        char ip[INET_ADDRSTRLEN];
        struct sockaddr_in *s_in = (struct sockaddr_in *) &ifr[i].ifr_addr;
        
        if (!inet_ntop(AF_INET, &s_in->sin_addr, ip, sizeof(ip))) 
        {
            //perror("inet_ntop");
            return -1;
        }
    
        //printf("%s - %s\n", ifr[i].ifr_name, ip);
        ip_set.insert(ip);
    }
    
    close(s);
    
    return 0;
    
}
int get_local_ip_addr(std::tr1::unordered_set<std::string> & ip_set, bool without_lo)
{
    int s;
    struct ifconf ifconf;
    struct ifreq ifr[50];
    int ifs;
    int i;
    
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) 
    {
        perror("socket");
        return -1;
    }
    
    ifconf.ifc_buf = (char *) ifr;
    ifconf.ifc_len = sizeof ifr;
    
    if (ioctl(s, SIOCGIFCONF, &ifconf) == -1) 
    {
        perror("ioctl");
        return -1;
    }
    
    ifs = ifconf.ifc_len / sizeof(ifr[0]);
    //printf("interfaces = %d:\n", ifs);
    for (i = 0; i < ifs; i++) 
    {
        if (without_lo)
        {
            if (strncmp(ifr[i].ifr_name, "lo", 2) == 0)
            {
                continue;
            }
        }
                
        char ip[INET_ADDRSTRLEN];
        struct sockaddr_in *s_in = (struct sockaddr_in *) &ifr[i].ifr_addr;
        
        if (!inet_ntop(AF_INET, &s_in->sin_addr, ip, sizeof(ip))) 
        {
            //perror("inet_ntop");
            return -1;
        }
    
        //printf("%s - %s\n", ifr[i].ifr_name, ip);
        ip_set.insert(ip);
    }
    
    close(s);
    
    return 0;
    

}

}//namespace common
}//namespace zonda

