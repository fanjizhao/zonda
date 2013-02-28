
#include <iostream>
#include "util/TimerService.h"
#include "thread/ThreadMgr.h"
#include "seda/IEventHandler.h"
#include "seda/Stage.h"
#include "util/TimerEvent.h"
#include "util/GlobalTimer.h"
#include "log/Logger.h"

using namespace std;
using namespace zonda::common;

class BizHandler: public zonda::common::IEventHandler
{
public:
    virtual int init(zonda::common::IStage* stage, void* param);
    virtual int handle_event(const zonda::common::IEvent* event);
private:
	zonda::common::IStage* m_stage;  
};


int BizHandler::init(zonda::common::IStage* stage, void* param)
{
	m_stage = stage;
    return 0;
}

int BizHandler::handle_event(const zonda::common::IEvent* e)
{
	TimerEvent* event = (TimerEvent*)e;
	
	
    std::cout << "Thread id: " << zonda::common::Thread::self_id() 
    	<< ", Handling the event, type = " << event->get_type() 
    	<< ", timer id = " << event->get_id() 
    	<< ", param: " << event->get_param() << std::endl;
	
	//std::cout << "jj" << std::endl;
	
	//TODO: free event
	delete event;
	

    return 0;
}


	
int main()
{
    zonda::logger::LoggerFactory::init("timer_test", "log.conf", false);
	TimerService my_timer;
	my_timer.start();

    zonda::common::Stage<BizHandler, zonda::common::EventQueue> biz_stage("biz_stage", 100000, 1);
    biz_stage.init(NULL);
    biz_stage.start();

	GlobalTimer::instance()->init();
	LOG_FATAL("Begin the test");
	for (int i=0; i<1; ++i)
	{
		//my_timer.schedule(biz_stage.get_sink(), i, i*1000, 1000);
		//my_timer.schedule(biz_stage.get_sink(), i, i*1000, 100);
		//my_timer.schedule(biz_stage.get_sink(), i, i*100);
		GlobalTimer::instance()->schedule(biz_stage.get_sink(), 1, 1000, 1000);
		//my_timer.schedule(biz_stage.get_sink(), 1, 1000, 1000);
		//LOG_DEBUG(i);
		//usleep(1000000); 
	}
	LOG_FATAL("End the test");
	for (int i=0; i<-1; ++i)
	{
		//my_timer.schedule(biz_stage.get_sink(), i, i*1000, 1000);
		//my_timer.schedule(biz_stage.get_sink(), i, i*1000, 100);
		my_timer.cancel_timer(i+1);
		//GlobalTimer::instance()->schedule(biz_stage.get_sink(), i, i*1000);
		//LOG_DEBUG(i);
		//usleep(1000000); 
	}
	LOG_FATAL("End the test--Cancel");
	
	//GlobalTimer::instance()->stop();

	ThreadMgr::instance()->wait_all();
}

