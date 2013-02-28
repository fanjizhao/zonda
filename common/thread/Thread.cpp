#include <sys/prctl.h>
#include <errno.h>
#include <cstring>
#include "Thread.h"
#include "ThreadMgr.h"
#include "log/Logger.h"



using namespace zonda::common;


void * thread_func(void *param)
{
    ThreadMgr::instance()->reg_thread(Thread::self_id());
    pthread_detach(pthread_self());
    Thread* t = (Thread*)param;
    
    //Set the thread name    
    prctl(PR_SET_NAME, t->get_name(),0,0,0);
    
    //LOG_DEBUG("Before the run()");
    pthread_attr_t Attr;
    size_t stack_size;
    pthread_attr_getstacksize(&Attr, &stack_size);//获取栈大小
    //LOG_DEBUG("stack size = " << stack_size);

    t->run();
    ThreadMgr::instance()->dereg_thread(Thread::self_id());
    return NULL;
}

pthread_t Thread::self_id()
{
    return pthread_self();
}


Thread::Thread()
{
    m_is_started = false;
    //Default thread size is 8MB
    m_stack_size = 8*1024*1024;
    m_thread_num = 1;
    m_active_count = 0;
    
}

void Thread::set_thread_name(const char* name)
{
    m_name = name;
}

int Thread::set_stack_size(int stack_size)
{
    m_stack_size = stack_size;
    return 0;
}

int Thread::set_thread_num(int thread_num)
{
    m_thread_num = thread_num;
    return 0;
}


int Thread::start()
{
    if (m_is_started)
    {
        return 0;
    }
    int r=0;
    for (int i=0; i<m_thread_num; ++i)
    {
        pthread_t pthread_id;
        pthread_attr_t thread_attr;
        pthread_attr_init(&thread_attr);
        pthread_attr_setstacksize(&thread_attr,104857600);

        r = pthread_create(&pthread_id, NULL, thread_func, (void*)this);
        if ( !r)
        {            
            m_is_started = true;
            m_thread_id_vector.push_back(pthread_id);            
            m_active_count++;
        }
        else
        {
            LOG_FATAL("Failed to call pthread_create, error:" << strerror(errno));
            return r;
        }
    }
    
    return r;
}

const char* Thread::get_name()
{
    return m_name.c_str();
}

