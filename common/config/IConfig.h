#ifndef ZONDA_COMMON_CONFIG_ICONFIG_H_
#define ZONDA_COMMON_CONFIG_ICONFIG_H_

#include <vector>
#include <string>
#include "seda/IEventSink.h"


namespace zonda {
namespace common {

using namespace std;

class IConfig
{
public:
    static const char *ZONDA_CONF_PATH;
    static const char *ZONDA_ROUTE_ADMIN_PATH;
    static const char *ZONDA_ROUTE_TABLE_PATH;
    static const char *ZONDA_ROUTE_STRATEGY_PATH;
    static const char *ZONDA_RUNTIME_PATH;
    static const char *ZONDA_SERVICE_GROUPING_PATH;

    static const int NODE_EPHEMERAL = 1; // mapping to ZOO_EPHEMERAL
    static const int NODE_SEQUENCE = 2; // mapping to ZOO_SEQUENCE
    
    virtual int init(const char *service_addr, int timeout) = 0;

    // flags can be NODE_EPHEMERAL or NODE_SEQUENCE or ORed together
    virtual int create_path(const char *path, const char *value, int valuelen,
                    int flags) = 0;

    virtual int delete_path(const char *path) = 0;

    // CONFIG_CHANGE_EVENT event will put into event_sink if event_sink is not NULL
    virtual int exists_path(const char *path, bool &existed,
                    IEventSink *event_sink = NULL, bool remove_watch = false) = 0;

    // value with length value_len need be created and freed by programmer outside
    // CONFIG_CHANGE_EVENT event will put into event_sink if event_sink is not NULL
    virtual int get_value(const char *path, char *value, int *value_len,
                  IEventSink *event_sink = NULL, bool remove_watch = false) = 0;

    virtual int set_value(const char *path, const char *value, int value_len) = 0;

    // CONFIG_CHANGE_EVENT event will put into event_sink if event_sink is not NULL
    virtual int get_children(const char *path, vector<string> &children,
                     IEventSink *event_sink = NULL, bool remove_watch = false) = 0;

    /* C++ style versions start */
    virtual int create_path(const string &path, const string &value, int flags) = 0;
    virtual int delete_path(const string &path) = 0;
    virtual int exists_path(const string &path, bool &existed,
                    IEventSink *event_sink = NULL, bool remove_watch = false) = 0;
    virtual int get_value(const string &path, string &value,
                  IEventSink *event_sink = NULL, bool remove_watch = false) = 0;
    virtual int set_value(const string &path, const string &value) = 0;
    virtual int get_children(const string &path, vector<string> &children,
                     IEventSink *event_sink = NULL, bool remove_watch = false) = 0;
    
};

#define CONFIG_PATH_PREFIX "/zonda/"



}//namespace common
}//namespace zonda

#endif //ZONDA_COMMON_CONFIG_ICONFIG_H_