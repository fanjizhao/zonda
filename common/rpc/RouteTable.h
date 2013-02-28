#ifndef ZONDA_COMMON_ROUTE_TABLE_H_
#define ZONDA_COMMON_ROUTE_TABLE_H_

#include <stdint.h>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <vector>
#include <string>
#include "util/Singleton.h"
#include "seda/IEventSink.h"
#include "IRpcService.h"

namespace zonda
{
namespace common
{
using namespace std;
using namespace std::tr1;

typedef uint16_t SERVICE_TYPE;
typedef int64_t INSTANCE_ID_TYPE;
typedef std::string INSTANCE_NAME_TYPE;
    
class ServiceAddr;   
class RouteTableImpl
{
public:
    struct ServiceItem
    {
        std::string instance_name;
        uint16_t service_type;
        int group_id;
        int64_t instance_id;
        std::vector<std::pair<std::string, uint16_t> > tcp_addr_list;
        std::vector<std::string> ipc_addr_list;
        int state;
        int service_seq;
        void update_service_seq();
        void reset();
    };
    
    struct compare_by_sequence
    {
        bool operator()(const ServiceItem& item1, const ServiceItem& item2)
        {
        	if (item1.service_seq < item2.service_seq)
        	{
        		//cout << get_path_seq(item1.instance_name) << "####" << get_path_seq(item2.instance_name) << endl;
        		return true;
        	}
        	return false;
        }
    };    
public:
    //! Load route info from the buff, if failed, the old info is not affected.
    //! This method can be called more than once as needed
    int load(const char* buff, int data_len);
    
    int load(const char* file_path);
    
    //! To get the real addr of the dst
    int get_real_addr(const ServiceAddr& responser,
        std::string& host, uint16_t &port);
        
    
    void set_local_listen_addr(vector<pair<string,uint16_t> >& tcp_addr_list, 
        vector<string>& ipc_addr_list);
    
    //!0 success, -1 no such service_type    
    int get_service_item_list(uint16_t service_type, vector<ServiceItem>& item_list);
protected:
    RouteTableImpl();
    
private:
    /*
    int get_real_addr_by_instance_id(
        const ServiceAddr& responser, 
        string& host, 
        uint16_t &port);*/
          
    bool is_local_ip(const string& ip);
    
    
    int update_service_map(const ServiceItem& item,
        unordered_map<INSTANCE_ID_TYPE, ServiceItem>& new_map); 
        
    void update_service_list(const ServiceItem& item, 
        vector< vector<ServiceItem> >& service_list);
        
    void update_local_service_list(const ServiceItem& item,
        vector< vector<ServiceItem> >& local_service_list);
        
    void update_local_host_service_list(const ServiceItem& item,
        vector< vector<ServiceItem> >& local_host_service_list);
        
    void update_group_list();

    void update_ipc_map(const ServiceItem& item);
    
    
    int choose_round_robin(const ServiceAddr& responser, 
        string& host, uint16_t &port);
    int choose_directly(const ServiceAddr& responser, 
        string& host, uint16_t &port);
    int choose_localhost(const ServiceAddr& responser, 
        string& host, uint16_t &port);
    int choose_master(const ServiceAddr& responser, 
        string& host, uint16_t &port);


    ThreadLock m_lock;
    
    
    unordered_map<INSTANCE_ID_TYPE, ServiceItem> m_service_map;
    vector< vector<ServiceItem> > m_service_list;
    
    //Three dimensional array
    //service_type, group_id,  
    vector< vector< vector<ServiceItem> > > m_group_list;
    
    //All services supported by this process
    vector< vector<ServiceItem> > m_local_service_list;
    
    //All services on local host    
    vector< vector<ServiceItem> > m_local_host_service_list;
    
    unordered_set<string> m_local_ip_set;
    
    vector<pair<string, uint16_t> > m_local_tcp_addr_list;
    vector<string> m_local_ipc_addr_list;
    
    uint16_t m_service_count;
    
    int m_service_round_robin[65535];
};

typedef Singleton<RouteTableImpl>  RouteTable;

}//namespace common
}//namespace zonda

#endif //ZONDA_COMMON_ROUTE_TABLE_H_
