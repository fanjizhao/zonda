#include "ThreadCond.h"

using namespace zonda::common;

ThreadCond::ThreadCond(ThreadLock& lock):
    m_mutex(lock.m_mutex)	
{
    
	pthread_cond_init(&m_cond, NULL);
}

ThreadCond::~ThreadCond()
{
    pthread_cond_destroy(&m_cond);
}

int ThreadCond::wait()
{
	int r = pthread_cond_wait(&m_cond, &m_mutex);
	return r;
}

int ThreadCond::time_wait(int millseconds)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    long nsec = ts.tv_nsec + millseconds*1000000;
    
    __time_t sec = nsec / 1000000000;
    ts.tv_nsec = nsec - sec*1000000000;
    ts.tv_sec += sec;
    
	int r = pthread_cond_timedwait(&m_cond, &m_mutex, &ts);
	return r;    
}

int ThreadCond::time_wait(const struct timespec& ts)
{
	int r = pthread_cond_timedwait(&m_cond, &m_mutex, &ts);
	return r;     
}

int ThreadCond::time_wait(const struct timeval& tv)
{
    struct timespec ts;
    ts.tv_sec = tv.tv_sec;
    ts.tv_nsec = tv.tv_usec*1000; 
	int r = pthread_cond_timedwait(&m_cond, &m_mutex, &ts);
	return r;     
}

int ThreadCond::signal()
{
	int r = pthread_cond_signal(&m_cond);
	return r;
}	

int ThreadCond::broadcast()
{
	int r = pthread_cond_broadcast(&m_cond);
	return r;
}	



