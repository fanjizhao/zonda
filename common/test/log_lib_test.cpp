#include "log/Logger.h"
																
int main(int argc, char* argv[]) {
	
    // init() method be invoked at first in your initialization program somewhere
    zonda::logger::LoggerFactory::init("test_module", "/home/yq/svn/zonda/trunk/log/conf/log4cpp.conf", false);

    // test null
    std::string* s;
    LOG_INFO("test null: " << s);

    // test format
    LOG_INFO("test format %06d " << 123);

    // test get_priority
    if( GET_LOG_PRIORITY() > 0  ) {
        LOG_INFO("get log priority: " << GET_LOG_PRIORITY());
    }
 
    // real logging sample
    // conf/log4cpp.conf
    LOG_DEBUG("something debug: " << 1);
    LOG_DEBUG("something debug: " << 2, "Sampler");
    
    LOG_INFO("something info: " << 3);
    LOG_INFO("something info: " << 2, "Sampler");
    
    LOG_WARN("something warn: " << 3);
    LOG_WARN("something warn: " << 3, "Sampler");
    
    LOG_ERROR("something error: " << 4); 
    LOG_ERROR("something error: " << 4, "Sampler"); 
    
    LOG_FATAL("something fatal: " << 4);  
    LOG_FATAL("something fatal: " << 4, "Sampler");  
    
    LOG_TRACE("123", "trace something: " << 4);  	  
    LOG_TRACE("123", "trace something: " << 4, "Sampler");  
     
    return 0;

}
