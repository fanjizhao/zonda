#include <iostream>
#include "seda/Stage.h"
#include "seda/IEventHandler.h"
#include "thread/ThreadMgr.h"
#include "thread/ThreadLock.h"
#include "seda/StageMgr.h"

using namespace zonda::common;

const int TEST_EVENT_COUNT = 4;

zonda::common::ThreadLock counter_lock;
int handled_event_num = 0;

int count_event()
{
	int r = 0;
	counter_lock.lock();
	++handled_event_num;
	if (handled_event_num == TEST_EVENT_COUNT)
	{
		r = -1;
	}
	//std::cout << "handled_event_num = " << handled_event_num << std::endl;
	counter_lock.unlock();
	
	return r;
	
}

class TestEvent: public zonda::common::IEvent
{
public:
	TestEvent(int data)
	{
		m_data = data;
	}
	int get_type() const
	{
		return 9999;
	}
	
	int get_data()
	{
		return m_data;
	}
private:
	int m_data;
};


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
	TestEvent* event = (TestEvent*)e;
	
	
    std::cout << "Thread id: " << zonda::common::Thread::self_id() 
    	<< ", Handling the event, type = " << event->get_type() 
    	<< ", data = " << event->get_data() << std::endl;
	
	//std::cout << "jj" << std::endl;
	
	//TODO: free event
	delete event;
	/*
    if (count_event())
    {
    	std::cout << "Finished handling all events" << std::endl;
    	//This statement works.
    	//m_stage->stop();
    	//exit(0);
    }*/
    return 0;
}

class EventProducer: public zonda::common::Thread
{
public:
	void run()
	{
		zonda::common::IStage* stage = zonda::common::StageMgr::instance()->get_stage("biz_stage");
		zonda::common::IEventSink* sink = stage->get_sink();

		for (int i=0; i<TEST_EVENT_COUNT; ++i)
	    {
	    	TestEvent* te = new TestEvent(i);
	    	//std::cout << "new TestEvent i=" << i << std::endl;
	    	
	    	sink->enqueue(te);
	    }
	}
	
	void stop()
	{
	}
};


void mem_new_test()
{
    char* p[10000];
    for (int i=0; i<10000; ++i)
    {
        p[i] = new char[1024*1024];
    }
}

class TestDispatcher: public EventDispatcher
{
public:
    virtual void enqueue(const IEvent* event)
    {
        cout << "@@@@@ Got a event, type:" << event->get_type() << endl;
        EventDispatcher::enqueue(event);
    }
};

int main()
{
    int times = 100000;
    zonda::logger::LoggerFactory::init("stage_test", "log.conf", false);
    
    /*
    zonda::common::EventQueue eq;
    eq.set_capacity(times);
    zonda::common::IEventSource* p = (zonda::common::IEventSource*)&eq;
    const zonda::common::IEvent* e;
    TestEvent* t[times];
    for (int i=0;i<times;++i)
    {
        t[i] = new TestEvent(i);
    }
    
    list<zonda::common::IEvent*> my_list;
    queue<IEvent*, list<IEvent*> > my_quque;
    vector<zonda::common::IEvent*> my_vector;
    IEvent* my_array[times];
    my_vector.reserve(times);
    ThreadLock my_lock;
    LOG_DEBUG("Before queue");
    size_t head = -1;
    size_t tail = -1;
    for (int i=0; i<times; ++i)
    {
        //LOG_DEBUG("Enqueue TestEvent " << i);
        eq.enqueue(t[i]);
        
        //my_lock.lock();
        //my_list.size();
        //my_list.push_back(t[i]);
        //my_quque.push(t[i]);
        //my_vector.push_back(t[i]);
        
        //my_array[i] = t[i];
        //head++;
        //tail++;  
        //my_lock.unlock();
        /*
        if (eq.time_dequeue(1000) == NULL)
        {
            LOG_ERROR("Should not get here");
        }
    }
    LOG_DEBUG("End queue");

    return 0;
    
	std::cout << "#########  Begin to test StageMgr ############" << std::endl;
    zonda::common::Stage<BizHandler, zonda::common::EventQueue> stage1("stage1", 100, 4);
    stage1.init(&stage1);    
    zonda::common::IStage* s1 = zonda::common::StageMgr::instance()->get_stage("stage1");
    std::cout << "get_stage: " << (s1? s1->get_name(): "NULL") << std::endl;
    
    
    //return 0;
    */
    
	std::cout << "#########  Begin to test stage ############" << std::endl;
    	
    zonda::common::Stage<BizHandler, zonda::common::EventQueue> biz_stage("biz_stage", 10000, 4);
    TestDispatcher td;
    
    biz_stage.init(NULL);
    biz_stage.set_event_dispatcher(&td);
    biz_stage.start();
    
    EventProducer event_producer;
    event_producer.set_thread_name("Event_Producer");
    event_producer.set_thread_num(2);
    event_producer.start();
    
    zonda::common::ThreadMgr::instance()->wait_all();
}

