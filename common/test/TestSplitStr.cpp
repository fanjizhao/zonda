#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include "../util/string_ex.h"

using namespace std;
using namespace zonda::common;

int main(int argc, char *argv[]) {
    string s = "dragoon_path=/usr/alisys/dragoon\ndragoon_ip=10.20.149.113\ndragoon_port=13888";
    map<string, string> str_map;
    split_str(s, str_map);
    for (map<string, string>::const_iterator iter = str_map.begin();
         iter != str_map.end(); ++iter) {
        printf("%s=%s\n", iter->first.c_str(), iter->second.c_str());
    }
    return 0;
}

