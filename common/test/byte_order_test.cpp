
#include "util/byte_order.h"
#include "log/Logger.h"
#include "rpc/RpcHead.h"
#include "rpc/TransHead.h"
#include "rpc/RouteStrategy.h"

using namespace zonda::common;

int main()
{
    //zonda::logger::LoggerFactory::init("zmq_trans_test", "log.conf", false);
    
    uint32_t my_32 = 10;
    uint64_t my_64 = 1000;
    RpcHead rpc_head;
    uint8_t buff[1024];
    for (int i=0; i<1000000; ++i)
    {
        rpc_head.hton_serialize(buff);
    }
    
    cout << "__BYTE_ORDER:" << __BYTE_ORDER <<endl;
    cout << "__BIG_ENDIAN:" << __BIG_ENDIAN << endl;
    
    ServiceInstanceId id;
    //id.set(6, "10.20.153.132", 5000);
    id.set_by_str("1.10.33.145.17.5000");
    //id.set(730024117138948096L);
    //id.value = my_ntohll(id.value);
    /*
    for (int i=0; i<8; ++i)
    {
        printf("%02x ", id.ba[i]);
    }
    printf("\n");
    */
    printf("str value:%s\n", id.to_str().c_str()); 
    cout << "digital value:" << id.get_value() << endl; 
    //string id_str = id.to_str();
    //id.set_by_str(id_str);
    
    /*
    printf("ip:%d port:%d type:%d \n", ntohl(id.ip), 
        ntohs(id.port), ntohs(id.service_type));*/
    char ip[20];
    uint16_t port;
    id.get_addr(ip, port);
    cout << "ip:" << ip << " port:" << port << endl;
    cout << "sizeof(ServiceAddr) = " << sizeof(ServiceAddr) << endl;
    cout << "sizeof(TransHead) = " << sizeof(TransHead) << endl;
    cout << "sizeof(RpcHead) = " << sizeof(RpcHead) << endl;
    return 0;
}
