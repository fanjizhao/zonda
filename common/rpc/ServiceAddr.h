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

//!��Ҫ��дserviceType, objId, objId�������������instanceId
DIRECT

//!��Ҫ��дserviceType, groupId,����������͵���group�����master������
MASTER_SLAVE

//!��Ҫ��дserviceType, groupId, objectIdд��0������������͵���group�����slave������
//!����ж��slave��������Ԥ�����õ��㷨(���Ǹ���ƽ�⣬ ����ʵ����state)�����������ĸ�slave
SLAVE


//!���еķ���ʵ���Եȣ�(���Ǹ���ƽ��)���ѡ���ĸ�����Ҫ��дserviceType
ROUND_ROBIN,

//!��Ҫ��дserviceType��objId, �����ݷ���ʵ����������objIdȡģ�����͵��ĸ�ʵ����ȥ
MODULO

//!��Ҫ��дserviceType��objId, ����objIdʹ��һ���Թ�ϣ�㷨�������͵��ĸ�ʵ����ȥ
CHASH

//!�͵������ϵ��Ǹ�����ʵ��������ж�������ѡȡһ��

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

