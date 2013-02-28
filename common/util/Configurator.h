#ifndef ZONDA_COMMON_CONFIGURATOR_H_
#define ZONDA_COMMON_CONFIGURATOR_H_

#include <string>
#include <map>
#include "IConfigurator.h"
#include "thread/ThreadLock.h"
#include "util/Singleton.h"

namespace zonda
{
namespace common
{

class ConfigImpl: public IConfigurator
{
public:
    //!load conf info from a buff, this method can be 
    //!called more than one time.
    //! @retval 0, load successfully, the old conf is discarded
    //! @retval -1, failed to load, the old conf is still available
    virtual int load(const char* buff, int buff_len);
    virtual int load(const char* file_path);

    
    //!return 0 indicates successfully getting the value 
    virtual int get_value(const char* key, std::string& value);
    virtual int get_value(const char* key, int64_t& value);
    virtual int get_value(const char* key, int32_t& value);
    virtual int get_value(const char* key, int16_t& value);
    virtual int get_value(const char* key, double& value);
    
    //!bool value can be string "true", it's converted bool true.
    //!Any other strings ared regarded as bool false
    virtual int get_value(const char* key, bool& value);     

protected:
    ConfigImpl(){} 
    virtual ~ConfigImpl() {}
private:
    std::map<std::string, std::string> m_map;
    zonda::common::ThreadLock m_lock;  
};

typedef Singleton<ConfigImpl> Configurator;

}//namespace common
}//namespace zonda
#endif //ZONDA_COMMON_CONFIGURATOR_H_
