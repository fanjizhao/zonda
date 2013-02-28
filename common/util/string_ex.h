#ifndef ZONDA_COMMON_STRING_EX_H_
#define ZONDA_COMMON_STRING_EX_H_

#include <string>
#include <vector>
#include <map>
#include <utility>


namespace zonda
{
namespace common
{

//To split a string into a vector.
//max_num is the maximum number we should try to find the delimiter.
//If we already find max_num delimiters, but we
//do not reach the end of the string, the rest of the string
//will be put into one element of str_vector. So at most, the str_vector
//would have max_num+1 elements. 
//If max_num is 0, we will try to find all delimiters to split the string.
void split_str(const std::string &s, 
    std::vector<std::string>& str_vector, 
    char delimiter, size_t max_num = 0);


//! 去掉字符串左侧的skip中的字符
std::string ltrim(const std::string& str, std::string skip = " ");


//! 去掉字符串左侧的skip中的字符
void ltrim(char *str, const char *skip = " ");


//! 去掉字符串右侧的skip中的字符
std::string rtrim(const std::string& str, std::string skip = " ");


//! 去掉字符串右侧的skip中的字符
void rtrim(char *str, const char *skip = " ");


//! 去掉字符串两侧的skip中的字符
void trim(char *str, const char *skip = " ");

std::string trim(const std::string &str, std::string skip = " ");

//! 转化为bool值(yes为true,其他为false)
bool to_bool(const char *str);

//! 转化为bool值(yes为true,其他为false)
bool to_bool(const std::string &str);

//! 字符串转换为小写
std::string lower(std::string str);


//! 字符串转换为小写
void lower(char *str);


void str_to_tcp_addr(const std::string& str, std::pair<std::string, uint16_t>& tcp_addr); 


void replace(std::string &str, const std::string &old_sub, long new_sub);

void replace(std::string &str, const std::string &old_sub, const std::string &new_sub);

//! 处理类似properties文件的字符串，将其转为一个map结构。\r\n都可作为换行标识，key和value前后的\t和空格会被trim
void split_str(const std::string &s, std::map<std::string, std::string>& str_map);

}//end namespace common
}//end namespace zonda

#endif //ZONDA_COMMON_STRING_EX_H_

