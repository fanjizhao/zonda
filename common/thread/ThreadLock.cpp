#include <iostream>
#include "ThreadLock.h"

using namespace zonda::common;

ThreadLock::ThreadLock()	
{
	//m_mutex = PTHREAD_MUTEX_INITIALIZER;
	int rt = pthread_mutex_init(&m_mutex, NULL);
	//std::cout << "rt: " << rt << std::endl;
	//std::cout << "&mutex: " << &m_mutex << std::endl;
	//std::cout << "mutex: " << m_mutex << std::endl;
}

ThreadLock::~ThreadLock()
{
    pthread_mutex_destroy(&m_mutex);
}

int ThreadLock::lock()
{
	int r = pthread_mutex_lock(&m_mutex);
	return r;
}

int ThreadLock::unlock()
{
	int r = pthread_mutex_unlock(&m_mutex);
	return r;
}	




