#ifndef ZONDA_ROUTE_ADMIN_BIZ_HANDLER
#define ZONDA_ROUTE_ADMIN_BIZ_HANDLER

#include <tr1/unordered_map>
#include "seda/IEventHandler.h"
#include "config/ConfigChangeEvent.h"

using namespace std;
using namespace std::tr1;


class BizHandler: public zonda::common::IEventHandler
{
public:
    BizHandler();
    virtual int init(zonda::common::IStage* stage, void* param);
    virtual int handle_event(const zonda::common::IEvent* event);
    int handle_conf_change_event(zonda::common::ConfigChangeEvent* event);
    int handle_route_admin_change(zonda::common::ConfigChangeEvent* event);
    int handle_service_change(zonda::common::ConfigChangeEvent* event);
    int handle_route_admin_count_change(zonda::common::ConfigChangeEvent* event);
    int handle_route_info_change(zonda::common::ConfigChangeEvent* event);
    virtual ~BizHandler() {};
private:
    int update_route_table();
    int get_services_data(std::vector<std::string>& children_list);
    int64_t get_path_seq(const string& path);  
private:
    zonda::common::IStage* m_my_stage;
    zonda::common::IEventSink* m_my_sink;
    bool m_master;
    //!How many route admin instance there are 
    int m_route_admin_count;
    
    string m_my_ip;
    int64_t m_my_seq;
    
    unordered_map<string, string> m_service_map;
    vector<string> m_route_admin_list;
};

#endif //ZONDA_ROUTE_ADMIN_BIZ_HANDLER

