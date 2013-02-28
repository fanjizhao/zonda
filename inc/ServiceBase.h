#ifndef ZONDA_INC_SERVICE_BASE_H_
#define ZONDA_INC_SERVICE_BASE_H_
#include "rpc/RpcEventHandlerBase.h"
#include "seda/Stage.h"
#include "rpc/RpcService.h"
#include "log/Logger.h"
#include "rpc/ServiceStageMap.h"
#include "rpc/RouteTable.h"
#include "inc/MsgFactory.h"
#include "thread/ThreadMgr.h"
#include "util/TimerService.h"
#include "util/TimerEvent.h"
#include "inc/services.h"
#include "util/MsgPool.h"
#include "util/FsmMgr.h"
//#include "util/Configurator.h"
#include "log/Logger.h"
#include "config/Config.h"

namespace zonda
{

template <class BizHandlerType>
class ServiceBase
{
public:
    ServiceBase(const char* service_name);
    void reg_service(int service_type);
    int init();
    int start();
    int run();

protected:
    zonda::MsgFactory m_msg_factory;
    zonda::common::RpcService m_rpc;
    std::vector<int> m_service_list;
    zonda::common::Stage<BizHandlerType> m_biz_stage;
    std::string m_service_name; 
};

template <class BizHandlerType>
ServiceBase<BizHandlerType>::ServiceBase(const char* service_name):
    m_biz_stage(service_name, 100000), m_service_name(service_name)
{
    m_service_list.push_back(zonda::service_type::CLIENT);
}


template <class BizHandlerType>
void ServiceBase<BizHandlerType>::reg_service(int service_type)
{
    if (service_type == 0) return;
    m_service_list.push_back(service_type);
}

template <class BizHandlerType>
int ServiceBase<BizHandlerType>::init()
{
    int r = 0;


    //r = Configurator::instance()->load("echo_service.conf");
    //RouteTable::instance()->load("route.conf");
    
    //LOG_DEBUG("I am echo_service");
    r = zonda::common::Config::instance()->init(NULL);
    if (r != 0)
    {
        LOG_FATAL("Failed to init config");
        return 0;
    }
        
    zonda::common::RpcServiceParam rpc_param(&m_msg_factory);

    r = m_rpc.init(&rpc_param);
    if (r != 0)
    {
        LOG_FATAL("Failed to init rpc");
        return -1;
    }
    
    r = m_biz_stage.init(&m_rpc);
    if (r != 0)
    {
        LOG_FATAL("Failed to init biz_stage");
        return  -1;
    }
    
    for (int i=0; i<m_service_list.size(); ++i)
    { 
        r = m_rpc.set_stage_service(&m_biz_stage, m_service_list[i]);
        if (r != 0)
        {
            LOG_FATAL("Failed to call set_stage_service");
            return -1;
        }
    } 
    
    
    return 0;
}

template <class BizHandlerType>
int ServiceBase<BizHandlerType>::start()
{
    int r = 0;
    r = m_biz_stage.start();
    if (r != 0)
    {
        LOG_FATAL("Failed to start m_biz_stage");
        return -1;
    }
    
    r = m_rpc.start();
    if (r != 0)
    {
        LOG_FATAL("Failed to start m_rpc");
        return -1;
    }    
    return 0;
}

template <class BizHandlerType>
int ServiceBase<BizHandlerType>::run()
{
    int r = this->init();
    if (r != 0)
    {
        LOG_FATAL("Failed to init the service");
        return -1;
    }
    
    r = this->start();
    if (r != 0)
    {
        LOG_FATAL("Failed to start the service");
        return -1;
    }    
    zonda::common::ThreadMgr::instance()->wait_all();
}

    
}//namespace zonda

#endif //ZONDA_INC_SERVICE_BASE_H_
