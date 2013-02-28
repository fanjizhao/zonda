#ifndef ZONDA_LOGGER_PRIORITY_H
#define ZONDA_LOGGER_PRIORITY_H

namespace zonda { 
namespace logger {

class Priority {
  
public:

    /**
     * Predefined Levels of Priorities. These correspond to the
     * priority levels used by syslog(3).
     */
    typedef enum {EMERG  = 0,
                  FATAL  = 0,
                  ALERT  = 100,
                  CRIT   = 200,
                  ERROR  = 300,
                  WARN   = 400,
                  NOTICE = 500,
                  INFO   = 600,
                  DEBUG  = 700,
                  NOTSET = 800
    } Level;    
    
    /**
     * The type of Priority Values
     */    
    typedef int Value;
    
};
    
}
}
#endif
