
#include <errno.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include "Configurator.h"
#include "log/Logger.h"
#include "string_ex.h"

namespace zonda
{
namespace common
{
int ConfigImpl::load(const char* buff, int buff_len)
{
    m_lock.lock();
    std::string content(buff, buff_len);
    std::vector<std::string> lines;
    split_str(content, lines, '\n');
    std::map<std::string, std::string> new_map;
    for (size_t i=0; i<lines.size(); ++i)
    {
        std::string line = trim(lines[i]);
        if (line.size() == 0 || line[0] == '#')
        {
            continue;
        }
        std::vector<std::string> item;
        split_str(line, item, '=', 1);
        
        if (item.size() == 2)
        {
            //std::cout << item[0] << "#######" << item[1] << std::endl;
            new_map[item[0]] = item[1];
        }
        else if (item.size() == 1)
        {
            new_map[item[0]] = "";
        }

    }
    
    new_map.swap(m_map);
    
    m_lock.unlock();
    
    return 0;
}

int ConfigImpl::load(const char* file_path)
{
    std::ifstream config_file;
	config_file.open(file_path);
	if (!config_file.is_open())
	{
		LOG_ERROR("open file error, filename:" << file_path
			<< ", error:" << strerror(errno));
		return -1;
	}
	
    std::stringstream buffer;
    buffer << config_file.rdbuf();
    
    config_file.close();
    return load(buffer.str().c_str(), buffer.str().size());
}


//!return 0 indicates successfully getting the value 
int ConfigImpl::get_value(const char* key, std::string& value)
{
    int r = 0;
    m_lock.lock();
    std::map<std::string, std::string>::iterator it;
    it = m_map.find(key);
    if (it == m_map.end())
    {
        r = -1;
    }
    else
    {
        value = it->second;
    }
    m_lock.unlock();
    return r;
}

int ConfigImpl::get_value(const char* key, int64_t& value)
{
    std::string s;
    int r = get_value(key, s);
    if (r != 0) return r;
    value = strtoll(s.c_str(), NULL, 10);
    if (errno != 0) return -1;    
    return 0;
}

int ConfigImpl::get_value(const char* key, int32_t& value)
{
    std::string s;
    int r = get_value(key, s);
    if (r != 0) return r;
    value = strtol(s.c_str(), NULL, 10);
    if (errno != 0) return -1;    
    return 0;
}
int ConfigImpl::get_value(const char* key, int16_t& value)
{
    std::string s;
    int r = get_value(key, s);
    if (r != 0) return r;
    value = strtol(s.c_str(), NULL, 0);
    if (errno != 0) return -1;    
    return 0;
}
int ConfigImpl::get_value(const char* key, double& value)
{
    std::string s;
    int r = get_value(key, s);
    if (r != 0) return r;
    value = strtod(s.c_str(), NULL);
    if (errno != 0) return -1;    
    return 0;
}
//!bool value can be string "true", it's converted bool true.
//!Any other strings ared regarded as bool false
int ConfigImpl::get_value(const char* key, bool& value)
{
    std::string s;
    int r = get_value(key, s);
    if (r != 0) return r;
    value = to_bool(s);
  
    return 0;
}   



}//namespace common
}//namespace zonda

