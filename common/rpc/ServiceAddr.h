#ifndef ZONDA_COMMON_SERVICE_ADDR_H_
#define ZONDA_COMMON_SERVICE_ADDR_H_

#include <arpa/inet.h>
#include "util/byte_order.h"
#include "rpc/RouteStrategy.h"
#include "ServiceInstanceId.h"

namespace zonda
{
namespace common
{


struct ServiceState
{
    enum
    {
        //In Service
        INS = 1,
    };
};

/*
MappingStrategy

//!需要填写serviceType, objId, objId将被看做服务的instanceId
DIRECT

//!需要填写serviceType, groupId,将会把请求送到该group里面的master服务上
MASTER_SLAVE

//!需要填写serviceType, groupId, objectId写成0，将会把请求送到该group里面的slave服务上
//!如果有多个slave，将按照预先设置的算法(考虑负载平衡， 服务实例的state)来决定发给哪个slave
SLAVE


//!所有的服务实例对等，(考虑负载平衡)随机选择哪个。需要填写serviceType
ROUND_ROBIN,

//!需要填写serviceType，objId, 将跟据服务实例个数，对objId取模决定送到哪个实例上去
MODULO

//!需要填写serviceType，objId, 将对objId使用一致性哈希算法，决定送到哪个实例上去
CHASH

//!送到本机上的那个服务实例，如果有多个，随机选取一个

LOCALHOST
//!
*/

struct ServiceAddr
{
    uint16_t service_type;
    uint16_t group_id;
    int64_t obj_id;
    int64_t instance_id;
    //0:master 1:slave
    int16_t role;
    int16_t state;
    ServiceAddr():
        service_type(0),
        group_id(0),
        obj_id(0),
        instance_id(0),
        role(0),
        state(ServiceState::INS)    
    {
    }
    
    void hton()
    {
        service_type = htons(service_type);
        group_id = htons(group_id);
        obj_id = my_htonll(obj_id);
        //instance_id = my_htonll(instance_id);
        ServiceInstanceId id;
        id.set(instance_id);
        id.hton();
        instance_id = id.get_value();
        role = htons(role);
        state = htons(state);
    }
    
    void ntoh()
    {
        service_type = ntohs(service_type);
        group_id = ntohs(group_id);
        obj_id = my_ntohll(obj_id);
        //instance_id = my_ntohll(instance_id);
        ServiceInstanceId id;
        id.set(instance_id);
        id.ntoh();
        instance_id = id.get_value();        
        role = ntohs(role);
        state = ntohs(state);
    }
};

}//namespace common
}//namespace zonda

#endif //ZONDA_COMMON_SERVICE_ADDR_H_

