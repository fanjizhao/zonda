
#ifndef ZONDA_COMMON_ROUTE_STRATEGY_H_
#define ZONDA_COMMON_ROUTE_STRATEGY_H_


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
    

class RouteStrategyImpl
{
public:
    enum
    {
        UNSUPPORTED = 0,   
        DIRECT = 1,
        MODULO = 2,
        CHASH = 3,
        ROUND_ROBIN = 4,
        LOCALHOST = 5,
        MASTER_SLAVE = 6,
        MAX_STRATEGY
                
    };
public:
    int load(const char* buff, int buff_len);
    int get_route_strategy(uint16_t service_type, uint16_t& strategy);
    static const char* get_strategy_str(uint16_t strategy);

private:
    static const char* m_strategy_str[MAX_STRATEGY+1];
    ThreadLock m_lock;
    vector<uint16_t> m_vector;
};

typedef Singleton<RouteStrategyImpl>  RouteStrategy;

} //namespace common
} //namespace zonda
#endif //ZONDA_COMMON_ROUTE_STRATEGY_H_
