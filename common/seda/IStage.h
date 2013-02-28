#ifndef ZONDA_COMMON_ISTAGE_H_
#define ZONDA_COMMON_ISTAGE_H_

#include <string>
#include "IEventSink.h"
#include "EventDispatcher.h"

namespace zonda
{
namespace common
{

//! Each stage implements the interface
struct IStage
{
    //!This enum type describes how multithreads work in a stage.
    enum StageThreadMode
    {
        //!All threads shared one event handler object. 
        //!That means the handler must be reentrant. 
        //!In other words, it must be thread-safe. 
        SHARED,     
        
        //! Each thread holds one event handler object.
        EXLUSIVE 
    };
    
    

    //! To init a stage.
    virtual int init(void *handler_param) = 0; 
    virtual void set_event_dispatcher(EventDispatcher* dispatcher) = 0;
            	
    virtual const char* get_name() = 0;
    virtual IEventSink* get_sink() = 0;
    virtual uint16_t get_id() = 0;
    virtual int start() = 0;
    
    
    //! stop all the threads in this stage
    virtual void stop() = 0;
    virtual ~IStage() {};
    
    //! Just for debug
    virtual void enable_trace() = 0;
    virtual void disable_trace() = 0;
};

} //namespace common
} //namespace zonda

#endif //ZONDA_COMMON_ISTAGE_H_
