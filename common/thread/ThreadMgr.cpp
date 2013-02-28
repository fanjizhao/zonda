#include <unistd.h>
#include <iostream>
#include "ThreadMgr.h"

using namespace zonda::common;

ThreadMgrImpl::ThreadMgrImpl()
{
}

void ThreadMgrImpl::reg_thread(pthread_t tid)
{
    m_lock.lock();
    m_thread_id_set.insert(tid);
    //std::cout << "reg_thread active_count:" << m_thread_id_set.size() << std::endl;
    m_lock.unlock();
}

void ThreadMgrImpl::dereg_thread(pthread_t tid)
{
    m_lock.lock();
    m_thread_id_set.erase(tid);
    //std::cout << "dereg_thread active_count:" << m_thread_id_set.size() << std::endl;
    m_lock.unlock();
}

int ThreadMgrImpl::active_count()
{
    int thread_count = 0;
    m_lock.lock();
    thread_count = m_thread_id_set.size();        
    m_lock.unlock();
    
    return thread_count;
}

void ThreadMgrImpl::wait_all()
{
    int thread_count = 0;
    while (1)
    {
        //NOTE:Must sleep before to call active_count.
        //Otherwise, say if the main thread is faster than all
        //the other threads, active_count returns 0, the main thread
        //exists, but the other threads have not started yet.
        sleep(1);
        thread_count = active_count();
        //std::cout << "thread_count:" << thread_count << std::endl;     
        if (thread_count == 0)        
        {
			break;
        }
    }
}


