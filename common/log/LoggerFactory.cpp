#include "LoggerFactory.h"
#include "Logger.h"
#include "LoggerStream.h"

using namespace std;

namespace zonda 
{
namespace logger 
{
  
    map<string, Logger*> LoggerFactory::m_logger_map;
    bool LoggerFactory::m_active = false;  
    bool LoggerFactory::m_show_hostname = false;
    string LoggerFactory::m_module = "default";
    string LoggerFactory::m_hostname = "127.0.0.1";
      
    /**
     * Zonda logger container initialization, always by configuration file.
     */    
    void LoggerFactory::init(const string& module, const string& conf, bool show_hostname) 
    {
        try {
            if (!LoggerFactory::m_active) 
            {
                //std::cout << "Loading logger configuration: "<< conf << std::endl; 
                log4cpp::PropertyConfigurator::configure(conf);
                m_module = module;
                m_active = true;
                m_show_hostname = show_hostname;
                char hostname[255]; 
                gethostname(hostname, sizeof(hostname));
                m_hostname = std::string(hostname);
                //std::cout << "Load logger configuration completed" << std::endl;
            }
        } 
        catch (log4cpp::ConfigureFailure& f) 
        {
            std::cout << "Configure error: " << f.what() << " and using default setting" << std::endl;
            std::cout << "All log info will be written to /tmp/log4cpp_default.log" << std::endl;
                
            log4cpp::Appender* appender = new log4cpp::FileAppender("default", "/tmp/log4cpp_default.log");
            log4cpp::PatternLayout* layout = new log4cpp::PatternLayout(); 
            layout->setConversionPattern("%m%n");           
            appender->setLayout(layout);
            log4cpp::Category& root = log4cpp::Category::getRoot();
            root.setAdditivity(true);
            root.setAppender(appender);
            root.setPriority(log4cpp::Priority::INFO);
                    
        }
    }     
    
    /**
     * Zonda logger container initialization, always by configuration file.
     */    
    void LoggerFactory::init(const string& module) {
       init(module, "../conf/log4cpp.conf", false);
    }
    
    /**
     * Fetch latest configuration info
     */ 
    void LoggerFactory::on_configuration_changed() {
        // TODO
    }  
    
    /**
     * Return root logger 
     */
    Logger* LoggerFactory::instance() {  
        if (m_logger_map["root"] == NULL)
        {
            log4cpp::Category& root = log4cpp::Category::getRoot();
            Logger* logger = new Logger(root);
            m_logger_map["root"] = logger;
            return logger;                   
        } 
        else 
        {
            return m_logger_map["root"];
        }
    }

    /**
     * Return logger with specific name
     */
    Logger* LoggerFactory::instance(const string& name) {
        if (m_logger_map[name] == NULL) 
        {
            log4cpp::Category& category = log4cpp::Category::getInstance(name);
            Logger* logger = new Logger(category);   
            m_logger_map[name] = logger;
            return logger;
        } 
        else 
        {
            return m_logger_map[name];  
        }
    }
    
    /**
     * Whether display hostname for each logging string
     */ 
    bool LoggerFactory::show_hostname() {
        return m_show_hostname;
    }    
    
    /**
     * Return logging factory hostname
     */
    std::string& LoggerFactory::get_hostname() {
        return m_hostname;
    }   
    
    /**
     * Return logging factory module
     */
    std::string& LoggerFactory::get_module() {
        return m_module;
    }
    
    /**
     * Reset program mudule name
     */ 
    void LoggerFactory::set_module(const string& _module) {
        LoggerFactory::m_module = _module;
    }
    
}
}

