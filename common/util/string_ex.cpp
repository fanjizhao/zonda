#include <ctype.h>
#include <cstring>
#include <sstream>
#include "string_ex.h"
namespace zonda
{
namespace common
{

void split_str(const std::string &s, std::vector<std::string>& str_vector,
    char delimiter, size_t max_num)
{
    size_t pos = 0;
    while(pos<s.size())
    {
        size_t pos1 = s.find(delimiter,pos);
        if (pos1 != std::string::npos)
        {
                str_vector.push_back(s.substr(pos,pos1-pos));
                pos = pos1+1;
        }
        else
        {
                str_vector.push_back(s.substr(pos));
                break;
        }

        if (max_num != 0 && str_vector.size() == max_num)
        {
            str_vector.push_back(s.substr(pos));
            break;
        }
    }
}

std::string ltrim(const std::string &str, std::string skip)
{
	size_t pos;
	for (pos = 0; pos < str.length(); pos++)
	{
		if (std::string::npos == skip.find(str[pos]))
			break;
	}
	return str.substr(pos);
}


void ltrim(char *str, const char *skip)
{
	char s[2];
	s[1] = 0;

	size_t i;
	for (i = 0; i < strlen(str); i++)
	{
		s[0] = str[i];
		if (NULL == strstr(skip, s))
		{
			break;
		}
	}

	int j = 0;
	for (size_t p = i; p < strlen(str) + 1; p++)
	{
		str[j++] = str[p];
	}
}

std::string rtrim(const std::string &str, std::string skip)
{
	size_t pos;
	for (pos = str.length() - 1; pos >= 0; pos--)
	{
		if (std::string::npos == skip.find(str[pos]))
			break;
	}
	return str.substr(0, pos + 1);
}

void rtrim(char *str, const char *skip)
{
	char s[2];
	s[1] = 0;

	for (int i = (int)strlen(str); i >= 0; i--)
	{
		s[0] = str[i];
		if (NULL == strstr(skip, s))
		{
			break;
		}
		else
		{
			str[i] = 0;
		}
	}
}

std::string trim(const std::string &str, std::string skip)
{
	return rtrim(ltrim(str, skip), skip);
}

void trim(char *str, const char *skip)
{
	rtrim(str, skip);
	ltrim(str, skip);
}

bool to_bool(const char *str)
{
	if (3 != strlen(str))
	{
		return false;
	}

	char tmp[5];
	strcpy(tmp, str);
	lower(tmp);

	if (0 != strcmp(tmp, "yes"))
	{
		return false;
	}

	return true;
}


bool to_bool(const std::string &str)
{
	return to_bool(str.c_str());
}


std::string lower(std::string str)
{
	std::string result = "";

	for (std::string::const_iterator c = str.begin(); c != str.end(); ++c)
	{
		result += tolower(*c);
	}

	return result;
}

void lower(char *str)
{
	int i = 0;
	while(str[i] != '\0')
	{
		str[i] = tolower(str[i]);
		i++;
	}
}

void str_to_tcp_addr(const std::string& str, std::pair<std::string, uint16_t>& tcp_addr)
{
    size_t pos = str.find(":");
    if (pos != std::string::npos)
    {
        tcp_addr.first = str.substr(0, pos);
        tcp_addr.second = atoi(str.substr(pos+1).c_str());
    }
    else
    {
        tcp_addr.first = str;                
    }
}

void replace(std::string &str, const std::string &old_sub, long new_sub) {
    std::stringstream ss;
    ss << new_sub;
    replace(str, old_sub, ss.str());
}

void replace(std::string &str, const std::string &old_sub, const std::string &new_sub) {
    size_t idx = 0;
    while (true) {
        idx = str.find(old_sub, idx);
        if (idx == std::string::npos) {
            break;
        }
        str.replace(idx, old_sub.size(), new_sub);
        idx += new_sub.size();
    }
}

void split_str(const std::string &s, std::map<std::string, std::string>& str_map)
{
    static const char *TRIM_STR = " \t";
    std::string str = s;
    replace(str, "\r", "\n");
    size_t pos = 0;
    while (pos < str.size())
    {
        size_t pos1 = str.find('\n', pos);
        std::string line;
        if (pos1 != std::string::npos) {
            line = str.substr(pos, pos1 - pos);
        } else {
            line = str.substr(pos);
        }
        trim(line, TRIM_STR);
        if (line.size() > 0) {
            size_t idx = line.find('=');
            if (idx != std::string::npos) {
                std::string key = trim(line.substr(0, idx), TRIM_STR);
                std::string value = trim(line.substr(idx + 1), TRIM_STR);
                str_map.insert(make_pair(key, value));
            }
        }

        if (pos1 != std::string::npos) {
            pos = pos1 + 1;
        } else {
            break;
        }
    }
}

}//end namespace common
}//end namespace zonda

