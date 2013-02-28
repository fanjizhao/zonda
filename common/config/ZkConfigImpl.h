#ifndef ZONDA_COMMON_CONFIG_ZK_CONFIG_H_
#define ZONDA_COMMON_CONFIG_ZK_CONFIG_H_

#include <zookeeper.h>
#include <map>
#include <set>

#include "IConfig.h"
#include "thread/ThreadLock.h"


#define ZOOKEEPER_ADDR_LIST_ENV_VAR "ZOOKEEPER_ADDR_LIST"

using namespace std;

namespace zonda
{
namespace common
{


class WatchValue 
{
public:
    set<IEventSink *> event_sinks;
};

class WatchChildren
{
public:
    set<IEventSink *> event_sinks;
};

class WatchExistence
{
public:
    set<IEventSink *> event_sinks;
};


class ZkConfigImpl: public IConfig
{
protected:
     ZkConfigImpl();   

public:    
    //!service_addr is a comma separated list of host:port pairs
    //!each corresponding to a ZooKeeper server.
    //!timeout, unit is ms
    virtual int init(const char *service_addr, int timeout = 3000);

    // flags can be NODE_EPHEMERAL or NODE_SEQUENCE or ORed together
    virtual int create_path(const char *path, const char *value, int valuelen,
                    int flags);

    virtual int delete_path(const char *path);

    // CONFIG_CHANGE_EVENT event will put into event_sink if event_sink is not NULL
    virtual int exists_path(const char *path, bool &existed,
                    IEventSink *event_sink = NULL, bool remove_watch = false);

    // value with length value_len need be created and freed by programmer outside
    // CONFIG_CHANGE_EVENT event will put into event_sink if event_sink is not NULL
    virtual int get_value(const char *path, char *value, int *value_len,
                  IEventSink *event_sink = NULL, bool remove_watch = false);

    virtual int set_value(const char *path, const char *value, int value_len);

    // CONFIG_CHANGE_EVENT event will put into event_sink if event_sink is not NULL
    virtual int get_children(const char *path, vector<string> &children,
                     IEventSink *event_sink = NULL, bool remove_watch = false);

    /* C++ style versions start */
    virtual int create_path(const string &path, const string &value, int flags);
    virtual int delete_path(const string &path);
    virtual int exists_path(const string &path, bool &existed,
                    IEventSink *event_sink = NULL, bool remove_watch = false);
    virtual int get_value(const string &path, string &value,
                  IEventSink *event_sink = NULL, bool remove_watch = false);
    virtual int set_value(const string &path, const string &value);
    virtual int get_children(const string &path, vector<string> &children,
                     IEventSink *event_sink = NULL, bool remove_watch = false);

public:
    void finish_connection(const clientid_t* client_id);
    void notify_children_change(const char* path);
    void notify_value_change(const char* path);
    void notify_path_deleted(const char* path);
private:
    ThreadLock      m_lock;
    zhandle_t*      m_zk_handle;
    bool            m_connected;
    clientid_t      m_zk_client_id;
    map<string, WatchValue> m_value_watcher;
    map<string, WatchChildren> m_children_watcher;
    map<string, WatchExistence> m_existence_watcher;
};

}//namespace common
}//namespace zonda

#endif