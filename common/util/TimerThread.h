#ifndef ZONDA_COMMON_TIMER_THREAD_H_
#define ZONDA_COMMON_TIMER_THREAD_H_


#include <queue>
#include <tr1/unordered_set>
#include "seda/IEventSink.h"
#include "seda/IEvent.h"
#include "thread/ThreadLock.h"
#include "thread/ThreadCond.h"
#include "thread/Thread.h"
#include "time_ex.h"


namespace zonda
{
namespace common
{




class TimerThread: public Thread
{
public:
	struct TimerItem
	{
		TimerItem();
		
		int id;
		
		//! When the timer is triggered
		struct timeval expire_time;
		
		//! 0 indicate this timer is not repeated
		int interval;
		
		int64_t param;
		
		IEventSink* sink;
	};
	
	struct LessTimerItem
	{
		//!If t1 < t2, return true, else return false
		bool operator()(const TimerItem* t1, const TimerItem* t2)
		{
			int r = timeval_compare(t1->expire_time, t2->expire_time);
			if (r < 0) return false;
			else return true;
		}
	};
	
public:
	TimerThread();
	virtual void run();
	
    int init(size_t max_timer_count);
    void set_default_handler_sink(IEventSink* handler_sink);
    void stop();
    void cancel_timer(int timer_id);
    void cancel_all_timer();
    
    
    //!To set a timer, a timer event will be thrown into the default handlerSink when the timer times out.
    //!If the default handler_sink is not set, the timer events are discarded.
   	//!@param interval represents how frequently the timer will be triggered, the unit is millsecond.
    //!@retval -1 Failed to schedule a timer 
    //!@retval >0 the timer_id,  which can be used to identify the timer
    int schedule(int64_t param, int delay, int interval = 0);

   
    //!To set a timer, a timer event will be thrown into the handler_sink when the timer times out.
    int schedule(IEventSink* handler_sink, int64_t param, int delay, int interval = 0);
    
    
private:
	//void handle_expired_timer();
	//int wait_next_timer();
	
	//!If this timer has been canceled, then remove it 
	//from the m_canceled_timers and return true,
	//else return false.
	inline bool check_timer_canceled(TimerItem* &item);
	
	//! it always increase and used to produce timer IDs
	int m_timer_seq;
    size_t m_max_timer_count;
	std::priority_queue<TimerItem*, std::vector<TimerItem*>, LessTimerItem> m_queue;
	//Save all canceled timer id
	std::tr1::unordered_set<int> m_canceled_timers;
	ThreadLock m_lock;
	ThreadCond m_cond;
	IEventSink* m_default_sink;
    bool m_stop;
    TimerItem* m_wait_item;
};


}//end namespace common
}//end namespace zonda

#endif //ZONDA_COMMON_TIMER_THREAD_H_

