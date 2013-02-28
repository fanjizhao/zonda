/**
 * @author fuping.yangfp@alibaba-inc.com
 */
#ifndef COMMON_UTIL_STACKTRACE_H_
#define COMMON_UTIL_STACKTRACE_H_

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sstream>

void print_stacktrace(const std::string &msg) {
    using namespace std;

    int size = 255;
    void *array[255];
    int stack_num = backtrace(array, size);
    char **stacktrace = backtrace_symbols(array, stack_num);
    stringstream addresses;
    for (int i = 1; i < stack_num - 2; ++i) {
        string line = stacktrace[i];
        size_t from = line.find_last_of('[');
        size_t to = line.find_last_of(']');
        if (from == string::npos || to == string::npos || from > to) {
            break;
        }
        addresses << " " << line.substr(from + 1, to - from - 1);
    }
    free(stacktrace);
    if (addresses.str().empty()) {
        printf("StackTrace: %s\n\tCan not find addresses.\n", msg.c_str());
        return;
    }

    char *bin_name = getenv("_");
    if (bin_name == NULL) {
        printf("StackTrace: %s\n\tCan not find running binary.\n", msg.c_str());
        return;
    }
    stringstream cmd;
    cmd << "addr2line -fsCe " << bin_name << addresses.str();

    char buff[1024];
    bool is_func_name = true;
    stringstream result;
    FILE *file = popen(cmd.str().c_str(), "r");
    while (file != NULL && !feof(file) && fgets(buff, sizeof(buff), file) != NULL) {
        size_t len = strlen(buff);
        if (len > 0 && buff[len - 1] == '\n') {
            buff[len - 1] = '\0';
            --len;
        }

        if (is_func_name) {
            result << "\tat " << buff;
            is_func_name = false;
        } else {
            result << " --> " << buff << endl;
            is_func_name = true;
        }
    }
    pclose(file);
    printf("StackTrace: %s\n%s\n", msg.c_str(), result.str().c_str());
}

#endif /* COMMON_UTIL_STACKTRACE_H_ */

