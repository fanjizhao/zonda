#ifndef COMMON_THREAD_H_
#define COMMON_THREAD_H_

#include <pthread.h>
#include <vector>
#include <string>


namespace zonda
{
namespace common
{



//!OS thread or thread pool wrapper.
class Thread
{
    
public:
    static pthread_t self_id();

public:
	Thread();
	
	
	void set_thread_name(const char* name);
	
	
	//! @parm thread_num how many OS threads will be create after start() is called 
	int set_thread_num(int thread_num);
	
	//! @param stack_size how many bytes the thread stack size is
	int set_stack_size(int stack_size);
	
	//! To start all the threads
	//! @return If all threads are created successfully, return 0, otherwise
	//! the return value of pthread_create.
	int start();
	
	
	virtual void stop() = 0;
	
	//! The subclass needs to implement the function, which is the 
	//! the real thread function. If the thread_num > 1, please ensure
	//! the function is thread safe
	virtual void run() = 0;
	
	//! Return how many threads the thead object really has. May different
	//! from thread_num
	int active_count();
	
	virtual ~Thread() {};

	const char* get_name();
	
private:
    std::vector<pthread_t> m_thread_id_vector;
    int m_stack_size;
    int m_thread_num;
    int m_active_count;
    bool m_is_started;
    std::string m_name;
};



} //namespace common	
} //namespace zonda


#endif //COMMON_THREAD_H_
