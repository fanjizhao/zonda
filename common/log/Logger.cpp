#include "Logger.h"
#include "LoggerStream.h"

namespace zonda {
namespace logger {

    Logger::Logger(log4cpp::Category& category) : m_category(category) {
      this->m_trace_enabled = true;
    }

    Logger::~Logger() {

    }

    /** 
     * Used to provide logging mechanism for log stream, since stream buffer will be converted
     * into a string and invoke category->log(string); This method is private and not be 
     * accessed by outside program.
     */
    void Logger::log(log4cpp::Priority::Value priority, const std::string& msg) {
        this->m_category.log(priority, msg);
    }

    /**
     * should be check before calling real debug function to improve performance
     * e.g.
     * if (logger->is_debug_enabled()) {
     *     logger->debug("something");
     * }
     */
    bool Logger::is_debug_enabled() {
        return this->m_category.isPriorityEnabled(log4cpp::Priority::DEBUG);
    }
  
    /**
     * used to log constant description string as debug level
     * e.g. logger->debug("something");
     */
    void Logger::debug(const std::string& msg) {
        this->m_category.debug(msg);
    }

    /**
     * used to log variable description string as debug level
     * e.g. logger->debug_stream() << "test1:" << "1" << "test2:" << 2;
     */
    LoggerStream Logger::debug_stream() {
        return *(new LoggerStream(*this, log4cpp::Priority::DEBUG));       
    }
    
    /**
     * should be check before calling real info function to improve performance
     * e.g.
     * if (logger->is_info_enabled()) {
     *     logger->info("something");
     * }
     */
    bool Logger::is_info_enabled() {
        return this->m_category.isPriorityEnabled(log4cpp::Priority::INFO); 
    }

    /**
     * used to log constant description string as info level
     * e.g. logger->info("something");
     */
    void Logger::info(const std::string& msg) {
      this->m_category.info(msg);
    }

    /**
     * used to log variable description string as info level
     * e.g. logger->info_stream() << "test1:" << "1" << "test2:" << 2;
     */
    LoggerStream Logger::info_stream() {
        return *(new LoggerStream(*this, log4cpp::Priority::INFO));   
    }    

    /**
     * should be check before calling real warn function to improve performance
     * e.g.
     * if (logger->is_warn_enabled()) {
     *     logger->warn("something");
     * }
     */
    bool Logger::is_warn_enabled() {
        return this->m_category.isPriorityEnabled(log4cpp::Priority::WARN); 
    }

    /**
     * used to log constant description string as warn level
     * e.g. logger->warn("something");
     */
    void Logger::warn(const std::string& msg) {
        this->m_category.warn(msg);
    }

    /**
     * used to log variable description string as warn level
     * e.g. logger->warn_stream() << "test1:" << "1" << "test2:" << 2;
     */
    LoggerStream Logger::warn_stream() {
        return *(new LoggerStream(*this, log4cpp::Priority::WARN));     
    }

    /**
     * should be check before calling real error function to improve performance
     * e.g.
     * if (logger->is_error_enabled()) {
     *     logger->error("something");
     * }
     */
    bool Logger::is_error_enabled() {
        return this->m_category.isPriorityEnabled(log4cpp::Priority::ERROR);  
    }

    /**
     * used to log constant description string as error level
     * e.g. logger->error("something");
     */
    void Logger::error(const std::string& msg) {
        this->m_category.error(msg);  
    }

    /**
     * used to log variable description string as error level
     * e.g. logger->error_stream() << "test1:" << "1" << "test2:" << 2;
     */
    LoggerStream Logger::error_stream() {
        return *(new LoggerStream(*this, log4cpp::Priority::ERROR)); 
    }
    
    /**
     * should be check before calling real fatal function to improve performance
     * e.g.
     * if (logger->is_fatal_enabled()) {
     *     logger->fatal("something");
     * }
     */
    bool Logger::is_fatal_enabled() {
        return this->m_category.isPriorityEnabled(log4cpp::Priority::FATAL);  
    }

    /**
     * used to log constant description string as fatal level
     * e.g. logger->fatal("something");
     */
    void Logger::fatal(const std::string& msg) {
        this->m_category.fatal(msg);  
    }

    /**
     * used to log variable description string as fatal level
     * e.g. logger->fatal_stream() << "test1:" << "1" << "test2:" << 2;
     */
    LoggerStream Logger::fatal_stream() {
        return *(new LoggerStream(*this, log4cpp::Priority::FATAL)); 
    }
    
    /**
     * should be check before calling real error function to improve performance
     * e.g.
     * if (logger->is_trace_enabled()) {
     *     logger->trace("something");
     * }
     */
    bool Logger::is_trace_enabled() {
        return this->m_trace_enabled;
    }

    /**
     * used to log constant description string as trace level
     * e.g. logger->trace("something");
     */
    void Logger::trace(const std::string& msg) {
        this->m_category.emerg(msg);  
    }

    /**
     * used to log variable description string as error level
     * e.g. logger->trace_stream() << "test1:" << "1" << "test2:" << 2;
     */
    LoggerStream Logger::trace_stream() {
        return *(new LoggerStream(*this, log4cpp::Priority::EMERG)); 
    }
    
    /**
     * set particular trace log level
     */
    void Logger::set_trace(bool flag) {
        this->m_trace_enabled = flag;
    }
    
    /**
     * Returns the assigned Priority, if any, for this Category.
     * @return Priority - the assigned Priority
     */
    Priority::Value Logger::get_priority() {
        return this->m_category.getPriority();
    }   
    
}
}
