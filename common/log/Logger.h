#ifndef ZONDA_LOGGER_LOGGER_H
#define ZONDA_LOGGER_LOGGER_H

#include <log4cpp/Portability.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/LoggingEvent.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/CategoryStream.hh>
#include <log4cpp/threading/Threading.hh>
#include <log4cpp/convenience.h>

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>

#include "LoggerFactory.h"
#include "LoggerStream.h"
#include "Priority.h"

#define GET_LOG_PRIORITY(category...)                                                               \
    zonda::logger::LoggerFactory::instance(                                                         \
    													strcmp(""category, "") == 0 ? __FILE__ : ""category)->get_priority()	\

#define LOG_DEBUG(msg, category...)                                                                 \
    if (zonda::logger::LoggerFactory::instance(strcmp(""category, "") == 0 ? __FILE__ : ""category) \
        ->is_debug_enabled()) {                                                                     \
                                                                                                    \
        std::string _z_l_category = ""category;                                                     \
        struct timeval _z_l_tv;                                                                     \
        gettimeofday(&_z_l_tv, NULL);                                                               \
        struct tm * _z_l_tmp = localtime(&_z_l_tv.tv_sec);                                          \
        																																														\
        char _log_time_stamp[30];                                                                  	\
        sprintf(_log_time_stamp, "%02d%02d%02d %02d:%02d:%02d.%06d",																\
        												 (int) (_z_l_tmp->tm_year + 1900),																	\
        												 (int) (_z_l_tmp->tm_mon + 1),																			\
        												 (int) _z_l_tmp->tm_mday,																						\
        												 (int) _z_l_tmp->tm_hour,																						\
        												 (int) _z_l_tmp->tm_min,																						\
        												 (int) _z_l_tmp->tm_sec,																						\
        												 (int) _z_l_tv.tv_usec);																						\
																																																		\
                                                                                                    \
        zonda::logger::Logger* logger =                                                             \
            zonda::logger::LoggerFactory::instance(_z_l_category == "" ? __FILE__ : _z_l_category); \
            																																												\
        if (zonda::logger::LoggerFactory::show_hostname()) {                                        \
            logger->debug_stream()                                                                  \
                          << _log_time_stamp                                                        \
                          << " D ["                                                                 \
                          << zonda::logger::LoggerFactory::get_hostname()                           \
                          << ":"                                                                    \
                          << zonda::logger::LoggerFactory::get_module()                             \
                          << ":"                                                                    \
                          << _z_l_category                                                          \
                          << ":"                                                                    \
                          << __FILE__                                                               \
                          << ":"                                                                    \
                          << __LINE__                                                               \
                          << ":"                                                                    \
                          << __FUNCTION__                                                           \
                          << ":-"                                                                   \
                          << ":"                                                                    \
                          << pthread_self()                                                         \
                          << "] - "                                                                 \
                          << msg;                                                                   \
        } else {                                                                                    \
            logger->debug_stream()                                                                  \
                          << _log_time_stamp																												\
                          << " D ["                                                                 \
                          << ":"                                                                    \
                          << zonda::logger::LoggerFactory::get_module()                             \
                          << ":"                                                                    \
                          << _z_l_category                                                          \
                          << ":"                                                                    \
                          << __FILE__                                                               \
                          << ":"                                                                    \
                          << __LINE__                                                               \
                          << ":"                                                                    \
                          << __FUNCTION__                                                           \
                          << ":-"                                                                   \
                          << ":"                                                                    \
                          << pthread_self()                                                         \
                          << "] - "                                                                 \
                          << msg;                                                                   \
        }                                                                                           \
    };                                                                                              \

#define LOG_INFO(msg, category...)                                                                  \
    if (zonda::logger::LoggerFactory::instance(strcmp(""category, "") == 0 ? __FILE__ : ""category) \
        ->is_info_enabled()) {                                                                      \
                                                                                                    \
        std::string _z_l_category = ""category;                                                     \
        struct timeval _z_l_tv;                                                                     \
        gettimeofday(&_z_l_tv, NULL);                                                               \
        struct tm *_z_l_tmp = localtime(&_z_l_tv.tv_sec);                                           \
                                                                                                    \
        char _log_time_stamp[30];                                                                  	\
        sprintf(_log_time_stamp, "%02d%02d%02d %02d:%02d:%02d.%06d",																\
        												 (int) (_z_l_tmp->tm_year + 1900),																	\
        												 (int) (_z_l_tmp->tm_mon + 1),																			\
        												 (int) _z_l_tmp->tm_mday,																						\
        												 (int) _z_l_tmp->tm_hour,																						\
        												 (int) _z_l_tmp->tm_min,																						\
        												 (int) _z_l_tmp->tm_sec,																						\
        												 (int) _z_l_tv.tv_usec);																						\
																																																		\
				zonda::logger::Logger* logger =                                                             \
            zonda::logger::LoggerFactory::instance(_z_l_category == "" ? __FILE__ : _z_l_category); \
            																																												\
        if (zonda::logger::LoggerFactory::show_hostname()) {                                        \
            logger->info_stream()                                                                   \
                          << _log_time_stamp																												\
                          << " I ["                                                                 \
                          << zonda::logger::LoggerFactory::get_hostname()                           \
                          << ":"                                                                    \
                          << zonda::logger::LoggerFactory::get_module()                             \
                          << ":"                                                                    \
                          << _z_l_category                                                          \
                          << ":"                                                                    \
                          << __FILE__                                                               \
                          << ":"                                                                    \
                          << __LINE__                                                               \
                          << ":"                                                                    \
                          << __FUNCTION__                                                           \
                          << ":-"                                                                   \
                          << ":"                                                                    \
                          << pthread_self()                                                         \
                          << "] - "                                                                 \
                          << msg;                                                                   \
        } else {                                                                                    \
            logger->info_stream()                                                                   \
                          << _log_time_stamp																												\
                          << " I ["                                                                 \
                          << ":"                                                                    \
                          << zonda::logger::LoggerFactory::get_module()                             \
                          << ":"                                                                    \
                          << _z_l_category                                                          \
                          << ":"                                                                    \
                          << __FILE__                                                               \
                          << ":"                                                                    \
                          << __LINE__                                                               \
                          << ":"                                                                    \
                          << __FUNCTION__                                                           \
                          << ":-"                                                                   \
                          << ":"                                                                    \
                          << pthread_self()                                                         \
                          << "] - "                                                                 \
                          << msg;                                                                   \
        }        												 																														\
    };                                                                                              \
      
#define LOG_WARN(msg, category...)                                                                  \
    if (zonda::logger::LoggerFactory::instance(strcmp(""category, "") == 0 ? __FILE__ : ""category) \
        ->is_warn_enabled()) {                                                                      \
                                                                                                    \
        std::string _z_l_category = ""category;                                                     \
        struct timeval _z_l_tv;                                                                     \
        gettimeofday(&_z_l_tv, NULL);                                                               \
        struct tm *_z_l_tmp = localtime(&_z_l_tv.tv_sec);                                           \
                                                                                                    \
        char _log_time_stamp[30];                                                                  	\
        sprintf(_log_time_stamp, "%02d%02d%02d %02d:%02d:%02d.%06d",																\
        												 (int) (_z_l_tmp->tm_year + 1900),																	\
        												 (int) (_z_l_tmp->tm_mon + 1),																			\
        												 (int) _z_l_tmp->tm_mday,																						\
        												 (int) _z_l_tmp->tm_hour,																						\
        												 (int) _z_l_tmp->tm_min,																						\
        												 (int) _z_l_tmp->tm_sec,																						\
        												 (int) _z_l_tv.tv_usec);																						\
                                                                                                    \
        zonda::logger::Logger* logger =                                                             \
            zonda::logger::LoggerFactory::instance(_z_l_category == "" ? __FILE__ : _z_l_category); \
            																																												\
        if (zonda::logger::LoggerFactory::show_hostname()) {                                        \
            logger->warn_stream()                                                                   \
                          << _log_time_stamp																												\
                          << " W ["                                                                 \
                          << zonda::logger::LoggerFactory::get_hostname()                           \
                          << ":"                                                                    \
                          << zonda::logger::LoggerFactory::get_module()                             \
                          << ":"                                                                    \
                          << _z_l_category                                                          \
                          << ":"                                                                    \
                          << __FILE__                                                               \
                          << ":"                                                                    \
                          << __LINE__                                                               \
                          << ":"                                                                    \
                          << __FUNCTION__                                                           \
                          << ":-"                                                                   \
                          << ":"                                                                    \
                          << pthread_self()                                                         \
                          << "] - "                                                                 \
                          << msg;                                                                   \
        } else {                                                                                    \
            logger->warn_stream()                                                                   \
                          << _log_time_stamp																												\
                          << " W ["                                                                 \
                          << ":"                                                                    \
                          << zonda::logger::LoggerFactory::get_module()                             \
                          << ":"                                                                    \
                          << _z_l_category                                                          \
                          << ":"                                                                    \
                          << __FILE__                                                               \
                          << ":"                                                                    \
                          << __LINE__                                                               \
                          << ":"                                                                    \
                          << __FUNCTION__                                                           \
                          << ":-"                                                                   \
                          << ":"                                                                    \
                          << pthread_self()                                                         \
                          << "] - "                                                                 \
                          << msg;                                                                   \
        }                                                                                           \
    };                                                                                              \

#define LOG_ERROR(msg, category...)                                                                 \
    if (zonda::logger::LoggerFactory::instance(strcmp(""category, "") == 0 ? __FILE__ : ""category) \
        ->is_error_enabled()) {                                                                     \
                                                                                                    \
        std::string _z_l_category = ""category;                                                     \
        struct timeval _z_l_tv;                                                                     \
        gettimeofday(&_z_l_tv, NULL);                                                               \
        struct tm *_z_l_tmp = localtime(&_z_l_tv.tv_sec);                                           \
                                                                                                    \
        char _log_time_stamp[30];                                                                  	\
        sprintf(_log_time_stamp, "%02d%02d%02d %02d:%02d:%02d.%06d",																\
        												 (int) (_z_l_tmp->tm_year + 1900),																	\
        												 (int) (_z_l_tmp->tm_mon + 1),																			\
        												 (int) _z_l_tmp->tm_mday,																						\
        												 (int) _z_l_tmp->tm_hour,																						\
        												 (int) _z_l_tmp->tm_min,																						\
        												 (int) _z_l_tmp->tm_sec,																						\
        												 (int) _z_l_tv.tv_usec);																						\
                                                                                                    \
        zonda::logger::Logger* logger =                                                             \
            zonda::logger::LoggerFactory::instance(_z_l_category == "" ? __FILE__ : _z_l_category); \
            																																												\
        if (zonda::logger::LoggerFactory::show_hostname()) {                                        \
            logger->error_stream()                                                                  \
                          << _log_time_stamp																												\
                          << " E ["                                                                 \
                          << zonda::logger::LoggerFactory::get_hostname()                           \
                          << ":"                                                                    \
                          << zonda::logger::LoggerFactory::get_module()                             \
                          << ":"                                                                    \
                          << _z_l_category                                                          \
                          << ":"                                                                    \
                          << __FILE__                                                               \
                          << ":"                                                                    \
                          << __LINE__                                                               \
                          << ":"                                                                    \
                          << __FUNCTION__                                                           \
                          << ":-"                                                                   \
                          << ":"                                                                    \
                          << pthread_self()                                                         \
                          << "] - "                                                                 \
                          << msg;                                                                   \
        } else {                                                                                    \
            logger->error_stream()                                                                  \
                          << _log_time_stamp																												\
                          << " E ["                                                                 \
                          << ":"                                                                    \
                          << zonda::logger::LoggerFactory::get_module()                             \
                          << ":"                                                                    \
                          << _z_l_category                                                          \
                          << ":"                                                                    \
                          << __FILE__                                                               \
                          << ":"                                                                    \
                          << __LINE__                                                               \
                          << ":"                                                                    \
                          << __FUNCTION__                                                           \
                          << ":-"                                                                   \
                          << ":"                                                                    \
                          << pthread_self()                                                         \
                          << "] - "                                                                 \
                          << msg;                                                                   \
        }                                                                                           \
    };                                                                                              \
                                                                                                                                                                                                                             
#define LOG_FATAL(msg, category...)                                                                 \
    if (zonda::logger::LoggerFactory::instance(strcmp(""category, "") == 0 ? __FILE__ : ""category) \
        ->is_fatal_enabled()) {                                                                     \
                                                                                                    \
        std::string _z_l_category = ""category;                                                     \
        struct timeval _z_l_tv;                                                                     \
        gettimeofday(&_z_l_tv, NULL);                                                               \
        struct tm *_z_l_tmp = localtime(&_z_l_tv.tv_sec);                                           \
                                                                                                    \
        char _log_time_stamp[30];                                                                  	\
        sprintf(_log_time_stamp, "%02d%02d%02d %02d:%02d:%02d.%06d",																\
        												 (int) (_z_l_tmp->tm_year + 1900),																	\
        												 (int) (_z_l_tmp->tm_mon + 1),																			\
        												 (int) _z_l_tmp->tm_mday,																						\
        												 (int) _z_l_tmp->tm_hour,																						\
        												 (int) _z_l_tmp->tm_min,																						\
        												 (int) _z_l_tmp->tm_sec,																						\
        												 (int) _z_l_tv.tv_usec);																						\
                                                                                                    \
        zonda::logger::Logger* logger =                                                             \
            zonda::logger::LoggerFactory::instance(_z_l_category == "" ? __FILE__ : _z_l_category); \
            																																												\
        if (zonda::logger::LoggerFactory::show_hostname()) {                                        \
            logger->fatal_stream()                                                                  \
                          << _log_time_stamp																												\
                          << " F ["                                                                 \
                          << zonda::logger::LoggerFactory::get_hostname()                           \
                          << ":"                                                                    \
                          << zonda::logger::LoggerFactory::get_module()                             \
                          << ":"                                                                    \
                          << _z_l_category                                                          \
                          << ":"                                                                    \
                          << __FILE__                                                               \
                          << ":"                                                                    \
                          << __LINE__                                                               \
                          << ":"                                                                    \
                          << __FUNCTION__                                                           \
                          << ":-"                                                                   \
                          << ":"                                                                    \
                          << pthread_self()                                                         \
                          << "] - "                                                                 \
                          << msg;                                                                   \
        } else {                                                                                    \
            logger->fatal_stream()                                                                  \
                          << _log_time_stamp																												\
                          << " F ["                                                                 \
                          << ":"                                                                    \
                          << zonda::logger::LoggerFactory::get_module()                             \
                          << ":"                                                                    \
                          << _z_l_category                                                          \
                          << ":"                                                                    \
                          << __FILE__                                                               \
                          << ":"                                                                    \
                          << __LINE__                                                               \
                          << ":"                                                                    \
                          << __FUNCTION__                                                           \
                          << ":-"                                                                   \
                          << ":"                                                                    \
                          << pthread_self()                                                         \
                          << "] - "                                                                 \
                          << msg;                                                                   \
        }                                                                                           \
    };                                                                                              \
    
#define LOG_TRACE(tx_id, msg, category...)                                                          \
    if (zonda::logger::LoggerFactory::instance(strcmp(""category, "") == 0 ? __FILE__ : ""category) \
        ->is_trace_enabled()) {                                                                     \
                                                                                                    \
        std::string _z_l_category = ""category;                                                     \
        struct timeval _z_l_tv;                                                                     \
        gettimeofday(&_z_l_tv, NULL);                                                               \
        struct tm *_z_l_tmp = localtime(&_z_l_tv.tv_sec);                                           \
                                                                                                    \
        char _log_time_stamp[30];                                                                  	\
        sprintf(_log_time_stamp, "%02d%02d%02d %02d:%02d:%02d.%06d",																\
        												 (int) (_z_l_tmp->tm_year + 1900),																	\
        												 (int) (_z_l_tmp->tm_mon + 1),																			\
        												 (int) _z_l_tmp->tm_mday,																						\
        												 (int) _z_l_tmp->tm_hour,																						\
        												 (int) _z_l_tmp->tm_min,																						\
        												 (int) _z_l_tmp->tm_sec,																						\
        												 (int) _z_l_tv.tv_usec);																						\
                                                                                                    \
        zonda::logger::Logger* logger =                                                             \
            zonda::logger::LoggerFactory::instance(																									\
            								_z_l_category.compare("") == 0 ? __FILE__ : _z_l_category); 						\
            																																												\
        if (zonda::logger::LoggerFactory::show_hostname()) {                                        \
            logger->trace_stream()                                                                  \
                          << _log_time_stamp																												\
                          << " T ["                                                                 \
                          << zonda::logger::LoggerFactory::get_hostname()                           \
                          << ":"                                                                    \
                          << zonda::logger::LoggerFactory::get_module()                             \
                          << ":"                                                                    \
                          << _z_l_category                                                          \
                          << ":"                                                                    \
                          << __FILE__                                                               \
                          << ":"                                                                    \
                          << __LINE__                                                               \
                          << ":"                                                                    \
                          << __FUNCTION__                                                           \
                          << ":"                                                                    \
                          << tx_id                                                                  \
                          << ":"                                                                    \
                          << pthread_self()                                                         \
                          << "] - "                                                                 \
                          << msg;                                                                   \
        } else {                                                                                    \
            logger->trace_stream()                                                                  \
                          << _log_time_stamp																												\
                          << " T ["                                                                 \
                          << ":"                                                                    \
                          << zonda::logger::LoggerFactory::get_module()                             \
                          << ":"                                                                    \
                          << _z_l_category                                                          \
                          << ":"                                                                    \
                          << __FILE__                                                               \
                          << ":"                                                                    \
                          << __LINE__                                                               \
                          << ":"                                                                    \
                          << __FUNCTION__                                                           \
                          << ":"                                                                    \
                          << tx_id                                                                  \
                          << ":"                                                                    \
                          << pthread_self()                                                         \
                          << "] - "                                                                 \
                          << msg;                                                                   \
        }                                                                                           \
    };                                                                                              \


/*
#undef LOG_DEBUG
#undef LOG_TRACE
#undef LOG_INFO
#undef LOG_WARN
#undef LOG_ERROR
#undef LOG_FATAL

#define LOG_DEBUG(msg, category...) 
#define LOG_TRACE(msg, category...)
#define LOG_INFO(msg, category...)
#define LOG_WARN(msg, category...) std::cout << msg << std::endl;
#define LOG_ERROR(msg, category...) std::cout << msg << std::endl;
#define LOG_FATAL(msg, category...) std::cout << msg << std::endl;
*/
                                                                                                        
namespace zonda { 
namespace logger {

/** 
 * class forward declaration, used to supprt stream-style logging as below 
 * e.g. logger << "info1:" << "1" << "info2:" << 2;
 */
class LoggerStream;

/**
 * Core class of logging library, providing string-style and stream-style logging.
 * In fact, Logger acts as a wrapper of other third part components, such as
 * log4cpp, log4cplus and etc. In this version, log4cpp is used to as the default
 * implementation and lots of log4cpp declaration exist as private properties.
 */
class Logger {

public:

    friend class LoggerFactory;
    friend class LoggerStream;


    /**
     * should be check before calling real debug function to improve performance
     * e.g.
     * if (logger->is_debug_enabled()) {
     *     logger->debug("something");
     * }
     */
    virtual bool is_debug_enabled();

    /**
     * used to log constant description string as debug level
     * e.g. logger->debug("something");
     */
    virtual void debug(const std::string& msg);
    
    /**
     * used to log variable description string as debug level
     * e.g. logger->debug_stream() << "test1:" << "1" << "test2:" << 2;
     */
    virtual LoggerStream debug_stream();
    
    /**
     * should be check before calling real info function to improve performance
     * e.g.
     * if (logger->is_info_enabled()) {
     *     logger->info("something");
     * }
     */
    virtual bool is_info_enabled();
    
    /**
     * used to log constant description string as info level
     * e.g. logger->info("something");
     */
    virtual void info(const std::string& msg);
    
    /**
     * used to log variable description string as info level
     * e.g. logger->info_stream() << "test1:" << "1" << "test2:" << 2;
     */
    virtual LoggerStream info_stream();
    
    /**
     * should be check before calling real warn function to improve performance
     * e.g.
     * if (logger->is_warn_enabled()) {
     *     logger->warn("something");
     * }
     */
    virtual bool is_warn_enabled();
    
    /**
     * used to log constant description string as warn level
     * e.g. logger->warn("something");
     */
    virtual void warn(const std::string& msg);
    
    /**
     * used to log variable description string as warn level
     * e.g. logger->warn_stream() << "test1:" << "1" << "test2:" << 2;
     */
    virtual LoggerStream warn_stream();
    
    /**
     * should be check before calling real error function to improve performance
     * e.g.
     * if (logger->is_error_enabled()) {
     *     logger->error("something");
     * }
     */
    virtual bool is_error_enabled();
    
    /**
     * used to log constant description string as error level
     * e.g. logger->error("something");
     */
    virtual void error(const std::string& msg);
    
    /**
     * used to log variable description string as error level
     * e.g. logger->error_stream() << "test1:" << "1" << "test2:" << 2;
     */
    virtual LoggerStream error_stream();
    
    /**
     * should be check before calling real fatal function to improve performance
     * e.g.
     * if (logger->is_fatal_enabled()) {
     *     logger->fatal("something");
     * }
     */
    virtual bool is_fatal_enabled();
    
    /**
     * used to log constant description string as fatal level
     * e.g. logger->fatal("something");
     */
    virtual void fatal(const std::string& msg);
    
    /**
     * used to log variable description string as error level
     * e.g. logger->fatal_stream() << "test1:" << "1" << "test2:" << 2;
     */
    virtual LoggerStream fatal_stream();  
    
    /**
     * should be check before calling real error function to improve performance
     * e.g.
     * if (logger->is_trace_enabled()) {
     *     logger->trace("something");
     * }
     */
    virtual bool is_trace_enabled();
    
    /**
     * used to log constant description string as trace level
     * e.g. logger->trace("something");
     */
    virtual void trace(const std::string& msg);
    
    /**
     * used to log variable description string as error level
     * e.g. logger->trace_stream() << "test1:" << "1" << "test2:" << 2;
     */
    virtual LoggerStream trace_stream(); 
    
    /**
     * set particular trace log level
     */
    virtual void set_trace(bool flag); 
    
    /**
     * Returns the assigned Priority, if any, for this Category.
     * @return Priority - the assigned Priority
     */
    virtual Priority::Value get_priority();                   
    
private:      
    
    // should constructed by factory
    Logger(log4cpp::Category& m_category);
    
    virtual ~Logger();
    
    // used to provide logging mechanism for log stream
    virtual void log(log4cpp::Priority::Value priority, const std::string& msg); 
    
    // real log4cpp category
    log4cpp::Category& m_category;  
      
    // logger trace flag
    bool m_trace_enabled;    
    
};

}//namespace logger
}//namespace common
#endif
