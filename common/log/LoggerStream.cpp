#include "Logger.h"
#include "LoggerStream.h"

namespace zonda {
namespace logger {
 
    /**
     * logger reference is used to write buffer string to log appender
     * priority is bind to current stream instance
     */
    LoggerStream::LoggerStream(Logger& logger, log4cpp::Priority::Value priority) : 
        m_logger(logger), 
        m_priority(priority), 
        m_buffer(NULL) {
          
    }

    /**
     * flush buffer string to log appender
     */
    LoggerStream::~LoggerStream() {
        flush();
    }

    /**
     * flush buffer string to log appender
     */
    void LoggerStream::flush() {
        if (m_buffer) {
            get_logger().log(m_priority, m_buffer->str()); // here
            delete m_buffer;
            m_buffer = NULL;
        }
    }

    std::streamsize LoggerStream::width(std::streamsize wide) {
        if (get_priority() != log4cpp::Priority::NOTSET) {
            if (!m_buffer) {
                if (!(m_buffer = new std::ostringstream)) {
                    // XXX
                }
            }
        }
        return m_buffer->width(wide); 
    }

}
}

