#ifndef ZONDA_LOGGER_LOGGERFACTORY_H
#define ZONDA_LOGGER_LOGGERFACTORY_H

#include <log4cpp/Portability.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/LoggingEvent.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/CategoryStream.hh>
#include <log4cpp/threading/Threading.hh>
#include <log4cpp/convenience.h>
#include <log4cpp/PropertyConfigurator.hh>

#include <string>
#include <map>

using namespace std;

namespace zonda 
{
namespace logger 
{

class LoggerStream;

class Logger;

/**
 * Zonda logger initialization and singleton object factory class, providing three key
 * static method for client program. init() should be invoked at first.
 */
class LoggerFactory {

public:

    /**
     * Zonda logger container initialization, always by configuration file.
     * @param module 
     */
    static void init(const string& module);
    
    /**
     * Zonda logger container initialization, always by configuration file.
     */
    static void init(const string& module, const string& conf, bool show_hostname);
      
    /**
     * Fetch latest configuration info
     */ 
    static void on_configuration_changed();

    /**
     * Return root logger 
     */
    static Logger* instance();

    /**
     * Return logger with specific name
     */
    static Logger* instance(const string& name);
      
    /**
     * Whether display hostname for each logging string
     */ 
    static bool show_hostname();  
      
    /**
     * Return logging factory hostname
     */
    static string& get_hostname(); 
    
    /**
     * Return logging factory module
     */
    static string& get_module();
    	
    /**
     * Reset program mudule name
     */	
    static void set_module(const string& _module);

private:

    // used for performance
    static map<string, Logger*> m_logger_map;

    // indicate whether current zonda logger is active
    static bool m_active;
    
    // indicate whether display hostname for each logging string
    static bool m_show_hostname;
    
    // program hostname
    static string m_hostname;
    
    // module name
    static string m_module;
};

}
}
#endif
