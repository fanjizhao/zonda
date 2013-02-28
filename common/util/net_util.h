#ifndef ZONDA_COMMON_NET_UTIL_H_
#define ZONDA_COMMON_NET_UTIL_H_


#include <string>
#include <vector>
#include <set>
#include <tr1/unordered_set>
#include <stdio.h>
//#include <stropts.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/netdevice.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

namespace zonda 
{
namespace common
{
int get_local_ip_addr(std::vector<std::string> & ip_list, bool without_lo = false);


int get_local_ip_addr(std::set<std::string> & ip_set, bool without_lo = false);

int get_local_ip_addr(std::tr1::unordered_set<std::string> & ip_set, bool without_lo = false);

}//namespace common
}//namespace zonda

#endif //ZONDA_COMMON_NET_UTIL_H_
