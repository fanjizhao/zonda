#ifndef ZONDA_COMMON_ICONFIG_H_
#define ZONDA_COMMON_ICONFIG_H_

namespace zonda
{
namespace common
{

class IConfigurator
{
public:
    //!load conf info from a buff, this method can be 
    //!called more than one time.
    //! @retval 0, load successfully, the old conf is discarded
    //! @retval -1, failed to load, the old conf is still available
    virtual int load(const char* buff, int buff_len) = 0;
    virtual int load(const char* file_path) = 0;

    
    //!return 0 indicates successfully getting the value 
    virtual int get_value(const char* key, std::string& value) = 0;
    virtual int get_value(const char* key, int64_t& value) = 0;
    virtual int get_value(const char* key, int32_t& value) = 0;
    virtual int get_value(const char* key, int16_t& value) = 0;
    virtual int get_value(const char* key, double& value) = 0;
    //!bool value can be string "true", it's converted bool true.
    //!Any other strings ared regarded as bool false
    virtual int get_value(const char* key, bool& value) = 0;     
    
    virtual ~IConfigurator() {}
};

}//namespace common
}//namespace zonda
#endif //ZONDA_COMMON_ICONFIG_H_
