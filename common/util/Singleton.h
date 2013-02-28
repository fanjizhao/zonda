#ifndef COMMON_SINGLETON_H_
#define COMMON_SINGLETON_H_

#include "thread/ThreadLock.h"

namespace zonda
{
namespace common
{

template<class IMPL, class THREAD_LOCK = ThreadLock>
class Singleton: public IMPL
{
public:
    static Singleton<IMPL, THREAD_LOCK> * instance();

private:
    Singleton();
    static THREAD_LOCK* m_lock;   
    static Singleton<IMPL, THREAD_LOCK>* m_instance;
};

template<class IMPL, class THREAD_LOCK>
Singleton<IMPL, THREAD_LOCK>* Singleton<IMPL, THREAD_LOCK>::m_instance = NULL;

template<class IMPL, class THREAD_LOCK>
THREAD_LOCK* Singleton<IMPL, THREAD_LOCK>::m_lock = new THREAD_LOCK();


template<class IMPL, class THREAD_LOCK>
Singleton<IMPL, THREAD_LOCK>::Singleton()
{
    
}


template<class IMPL, class THREAD_LOCK>
Singleton<IMPL, THREAD_LOCK>* Singleton<IMPL, THREAD_LOCK>::instance()
{
    if (m_instance == NULL)
    {
        m_lock->lock();
        if (m_instance == NULL)
        {
            Singleton<IMPL, THREAD_LOCK>* obj = new Singleton<IMPL, THREAD_LOCK>();
            m_instance = obj;            
        }
        m_lock->unlock();
    }
    
    return m_instance;
}

} //namespace common	
} //namespace zonda

#endif //COMMON_SINGLETON_H_
