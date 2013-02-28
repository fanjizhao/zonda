#ifndef COMMON_THREADMGR_H_
#define COMMON_THREADMGR_H_

#include <pthread.h>
#include <set>
#include "util/Singleton.h"
#include "ThreadLock.h"
namespace zonda
{
namespace common
{



//!OS Manage all the threads except the main thread
class ThreadMgrImpl
{
    
public:
    void reg_thread(pthread_t tid);
    void dereg_thread(pthread_t tid);
    //! Return the threads num except the main thread
    int active_count();
    void wait_all();
protected:
    ThreadMgrImpl();

	
private:
    ThreadLock m_lock;
    std::set<pthread_t> m_thread_id_set;

};

typedef Singleton<ThreadMgrImpl, ThreadLock> ThreadMgr;

} //namespace common	
} //namespace zonda


#endif //COMMON_THREADMGR_H_
