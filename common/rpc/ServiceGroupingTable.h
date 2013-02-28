#ifndef ZONDA_COMMON_SERVICE_GROUPING_TABLE_H_
#define ZONDA_COMMON_SERVICE_GROUPING_TABLE_H_

#include <stdint.h>
#include <tr1/unordered_map>
#include <vector>
#include <string>
#include "util/Singleton.h"
#include "thread/ThreadLock.h"

namespace zonda 
{ 
namespace common 
{

using namespace std;
using namespace std::tr1;




typedef uint16_t SERVICE_TYPE;
typedef uint16_t GROUP_ID_TYPE;

class ServiceGroupingTableImpl
{
public:
    static uint16_t MAX_GROUP_COUNT;
public:
    int load(const char* data, int data_len);
    uint16_t get_group_id(uint16_t service_type, const std::string& ip);
private:
    ThreadLock m_lock;
    unordered_map<SERVICE_TYPE, unordered_map<string, GROUP_ID_TYPE> > m_map;      
};

typedef Singleton<ServiceGroupingTableImpl>  ServiceGroupingTable;
} //namespace common
} //namespace zonda


#endif //ZONDA_COMMON_SERVICE_GROUPING_TABLE_H_



