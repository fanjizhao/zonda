#include <unistd.h>
#include <pthread.h>
#include <iostream>

#include "thread/Thread.h"
#include "thread/ThreadMgr.h"


class MyThread: public zonda::common::Thread
{
public:
	void run()
	{
		for (int i=0;i<10;i++)
		{
			std::cout << "thread id:" << zonda::common::Thread::self_id() << ", i=" << i << std::endl;
			sleep(20);
		}
	}
	void stop()
	{
		
	}
};


int main()
{
	std::cout << "main thread id: " << zonda::common::Thread::self_id()  
	    << std::endl;
	MyThread my_thread;
	my_thread.set_thread_num(1);
	my_thread.set_thread_name("my_thread");
	my_thread.start();
	zonda::common::ThreadMgr::instance()->wait_all();	
}
