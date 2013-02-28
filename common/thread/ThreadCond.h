#ifndef COMMON_THREAD_COND_H_
#define COMMON_THREAD_COND_H_

#include <sys/time.h>
#include "ThreadLock.h"

namespace zonda
{
namespace common
{



class ThreadCond
{
public:
	ThreadCond(ThreadLock& lock);
	~ThreadCond();
	int wait();
	int time_wait(int millseconds);
	int time_wait(const struct timespec& ts);
	int time_wait(const struct timeval& tv);
	int signal();
	int broadcast();

private:
	pthread_mutex_t& m_mutex;
	pthread_cond_t m_cond;
};



} //namespace common	
} //namespace zonda


#endif //COMMON_THREAD_COND_H_
