#ifndef ZONDA_COMMON_LOCK_GUARD_H_
#define ZONDA_COMMON_LOCK_GUARD_H_

#include "ThreadLock.h"

namespace zonda
{
namespace common
{

class LockGuard
{
public:
    LockGuard(ThreadLock& lock)
    {
        m_lock = &lock;
        m_lock->lock();
    }
    ~LockGuard()
    {
        m_lock->unlock();
    }
private:
    ThreadLock* m_lock;
};

}//namespace common
}//namespace zonda

#endif