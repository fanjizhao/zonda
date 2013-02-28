#ifndef ZONDA_LOGGER_LOGGERSTREAM_H
#define ZONDA_LOGGER_LOGGERSTREAM_H

#include <log4cpp/Portability.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/LoggingEvent.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/CategoryStream.hh>
#include <log4cpp/threading/Threading.hh>
#include <log4cpp/convenience.h>

#include <string>
#include <map>

namespace zonda {
namespace logger {
    
class Logger;

/**
 * Support << operator and client program can log variables to same line, all the string
 * information will be stored in a internal buffer.
 * e.g. logger << "info1:" << "1" << "info2:" << 2;
 */
class LoggerStream {

public:

    LoggerStream(Logger& logger, log4cpp::Priority::Value priority);

    ~LoggerStream();

    void flush();

    template<typename T> 
    LoggerStream& operator<<(const T& t) {
        if (get_priority() != log4cpp::Priority::NOTSET) {
            if (!m_buffer) {
          if (!(m_buffer = new std::ostringstream)) {
          // XXX
                }
            }
            (*m_buffer) << t;
        }
        return *this;
    }

    template<typename T> 
    LoggerStream& operator<<(const std::string& t) {
        if (get_priority() != log4cpp::Priority::NOTSET) {
            if (!m_buffer) {
                if (!(m_buffer = new std::ostringstream)) {
                    // XXX
                }
            }
            (*m_buffer) << t;
        }
        return *this;
    }

    std::streamsize width(std::streamsize wide);

private:

    inline Logger& get_logger() { return m_logger; };

    inline log4cpp::Priority::Value get_priority() { return m_priority; };

    Logger& m_logger;    

    log4cpp::Priority::Value m_priority;

    std::ostringstream* m_buffer;
    
};

}
}
#endif

