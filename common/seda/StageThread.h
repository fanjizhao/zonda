#ifndef ZONDA_COMMON_STAGETHREAD_H_
#define ZONDA_COMMON_STAGETHREAD_H_

#include "thread/Thread.h"
#include "IEvent.h"
#include "EventQueue.h"
#include "IEventHandler.h"
#include "thread/ThreadCond.h"

namespace zonda
{
namespace common
{
    
class StageThread: public Thread
{
public:
    StageThread(IStage* self_stage);
    void set_event_handler(IEventHandler* handler);
    void set_event_source(IEventSource* event_source);
    void run();
    void stop();
    void enable_trace();
    void disable_trace();
private:
    IEventHandler* m_event_handler;
    IEventSource* m_event_source;
    bool m_stop;
    bool m_trace;
    std::string m_stage_name;
};



}//end namespace common
}//end namespace zonda

#endif //ZONDA_COMMON_STAGETHREAD_H_
