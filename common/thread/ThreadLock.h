#ifndef COMMON_THREAD_LOCK_H_
#define COMMON_THREAD_LOCK_H_

#include <pthread.h>



namespace zonda
{
namespace common
{
    
class ThreadLock
{
public:
    friend class ThreadCond;
	ThreadLock();
	~ThreadLock();
	int lock();
	int unlock();

private:
	pthread_mutex_t m_mutex;
};


} //namespace common	
} //namespace zonda

#endif //COMMON_THREAD_LOCK_H_
