#ifndef ZONDA_RPC_SERVICE_STAGE_MAP_H_
#define ZONDA_RPC_SERVICE_STAGE_MAP_H_

#include <stdint.h>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <set>
#include <vector>
#include <string>
#include "util/Singleton.h"
#include "seda/IEventSink.h"
#include "seda/IStage.h"


namespace zonda
{
namespace common
{

using namespace std;
using namespace std::tr1;

class ServiceStageMapImpl
{
public:
 
    
    //!a sink can have many services.
    //!a sink can have many addr
    //!a service can on many addr
    //! a service can't be on more than one sink
    
       
    int set_stage_service(IStage* request_stage,
        uint16_t service_type,
        vector<pair<string, uint16_t> >& tcp_addr_list,
        vector<string>& ipc_addr_list);

    //! Get the stage which is responsible this service_type
    IStage* get_stage(uint16_t service_type);
    
    bool is_local_service(int64_t instance_id);
    
    int set_local_listen_addr(vector<pair<string, uint16_t> >& tcp_addr_list);
    
    //Get self instance_id for sending a request, when invoker type is CLIENT
    int get_self_instance_id(uint16_t service_type, int64_t& instance_id);    
    
    void get_local_service_instance_list(vector<int64_t>& instance_list,
        bool with_client_id = false); 
    
protected:
    ServiceStageMapImpl();
    
    int send_route_info( 
        uint16_t service_type,
        vector<pair<string, uint16_t> >& tcp_addr_list,
        vector<string>& ipc_addr_list);
    
    
private:
  
    unordered_map<uint16_t, IStage*> m_service_stage_map;
    unordered_map<uint16_t, set<pair<string, uint16_t> > > m_service_tcp_map;
    unordered_map<uint16_t, unordered_set<string> > m_service_ipc_map;
    unordered_map<uint16_t, int64_t> m_service_type_instance_id_map;
    ThreadLock m_lock;
};

typedef Singleton<ServiceStageMapImpl, ThreadLock>  ServiceStageMap;

}//namespace common
}//namespace zonda

#endif //ZONDA_RPC_SERVICE_STAGE_MAP_H_
