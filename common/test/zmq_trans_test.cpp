#include <cstring>
#include "rpc/ZMQTrans.h"
#include "thread/ThreadMgr.h"
#include "log/Logger.h"
#include "seda/Stage.h"
#include "rpc/TransInEvent.h"

using namespace zonda::common;

/*
class BizHandler: public IEventHandler
{
public:
    BizHandler();
    virtual int init(zonda::common::IStage* stage, void* param);
    virtual int handle_event(const zonda::common::IEvent* event);
    virtual ~BizHandler() {}    
private:
    zonda::common::ITrans* m_trans;
    zonda::common::IStage* m_self_stage;
};

int BizHandler::handle_event(const zonda::common::IEvent* event)
{
    int type = event->get_type();
    if (type == EventType::TRANS_IN_EVENT)
    {
        
    }
}

*/
int main()
{
    int r = 0;
    zonda::logger::LoggerFactory::init("zmq_trans_test", "log.conf", false);
    
    zonda::common::TransParm param;
    param.tcp_addr_list.push_back(std::make_pair("0.0.0.0", 8888));
    param.tcp_addr_list.push_back(std::make_pair("127.0.0.1", 8889));
    
    param.ipc_addr_list.push_back("zmq_trans_test.ipc");
       
    zonda::common::ZMQTrans my_trans("test");

    r = my_trans.init(&param);
    LOG_DEBUG("init r=" << r);
    r = my_trans.start();
    LOG_DEBUG("start r=" << r);

    sleep(2);
    LOG_DEBUG("Begin sending");
    for(int i=0; i<10; ++i)
    {
        char* str = new char[50];
        sprintf(str, "Great China, %d", i);
        //my_trans.send_data(i, (uint8_t*)str, strlen(str), "127.0.0.1", 8888);
        my_trans.send_data(i, (uint8_t*)str, strlen(str), "zmq_trans_test.ipc");
    }
    
    zonda::common::ThreadMgr::instance()->wait_all();
}
