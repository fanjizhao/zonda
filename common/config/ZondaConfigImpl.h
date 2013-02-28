#ifndef ZONDA_COMMON_CONFIG_ZONDA_CONFIG_H_
#define ZONDA_COMMON_CONFIG_ZONDA_CONFIG_H_

#include <map>
#include <set>
#include <string>
#include <queue>
#include "seda/IEventSink.h"
#include "seda/Stage.h"
#include "util/ZMQTrans.h"
#include "ConfigChangeEvent.h"
#include "ConfigHandler.h"
#include "IConfig.h"

namespace zonda 
{
namespace common 
{

using namespace std;

class WatchValue {
public:
    WatchValue() : existed(false), value(NULL), value_len(0) {
    }
    ~WatchValue() {
        delete[] value;
    }
    set<IEventSink *> event_sinks;
    bool existed;
    char *value;
    int value_len;
};

class WatchChild {
public:
    set<IEventSink *> event_sinks;
    vector<string> children;
};

class ZondaConfigImpl: public IConfig {
    friend class ConfigHandler;

public:
    ZondaConfigImpl() :
        m_trans("config"),
        m_stage("config_stage", 10000) {
    }
    virtual ~ZondaConfigImpl() {
    }
    int init(const char *service_addr);
    void add_watch_connect(IEventSink *event_sink);
    void remove_watch_connect(IEventSink *event_sink);

    // flags can be NODE_EPHEMERAL or NODE_SEQUENCE or ORed together
    int create_path(const char *path, const char *value, int valuelen,
                    int flags);

    int delete_path(const char *path);

    // CONFIG_CHANGE_EVENT event will put into event_sink if event_sink is not NULL
    int exists_path(const char *path, bool &existed,
                    IEventSink *event_sink = NULL, bool remove_watch = false);

    // value with length value_len need be created and freed by programmer outside
    // CONFIG_CHANGE_EVENT event will put into event_sink if event_sink is not NULL
    int get_value(const char *path, char *value, int *value_len,
                  IEventSink *event_sink = NULL, bool remove_watch = false);

    int set_value(const char *path, const char *value, int value_len);

    // CONFIG_CHANGE_EVENT event will put into event_sink if event_sink is not NULL
    int get_children(const char *path, vector<string> &children,
                     IEventSink *event_sink = NULL, bool remove_watch = false);

    /* C++ style versions start */
    int create_path(const string &path, const string &value, int flags);
    int delete_path(const string &path);
    int exists_path(const string &path, bool &existed,
                    IEventSink *event_sink = NULL, bool remove_watch = false);
    int get_value(const string &path, string &value,
                  IEventSink *event_sink = NULL, bool remove_watch = false);
    int set_value(const string &path, const string &value);
    int get_children(const string &path, vector<string> &children,
                     IEventSink *event_sink = NULL, bool remove_watch = false);

private:
    int register_config_lib();
    int handle_req(int op_type, const char *path, bool watch,
                   int flags, const char *value, int value_len,
                   ConfigFsm *fsm, IEventSink *event_sink);
    void handle_res(const char *buff, int buff_len, int64_t fsm_id);
    void notify_data(const char *buff, int buff_len);
    void get_data_async(const char *path, int internal_op);
    void notify_watch(set<IEventSink *> &event_sinks, ConfigChangeEvent *e);

    void get_from_cache(WatchValue *&watch_value, bool need_value,
                        const string &path_s);
    void get_from_cache(WatchChild *&watch_child, const string &path_s);
    void update_to_cache(const string &path_s, const bool existed,
                         const char *value, int value_len,
                         const vector<string> *children, IEventSink *event_sink);
    void add_to_cache(const string &path_s, IEventSink *event_sink,
                      bool child_cache);
    void remove_from_cache(const string &path_s, IEventSink *event_sink,
                           bool child_cache);
    void remove_from_cache(const string &path_s);
    bool has_watch_child_cache(const string &path_s);
    bool has_watch_value_cache(const string &path_s);
    void get_event_sinks_from_watch_connect(set<IEventSink *> &event_sinks);
    void get_event_sinks_from_watch_child(const string &path_s,
                                          set<IEventSink *> &event_sinks);
    void get_event_sinks_from_watch_value(const string &path_s,
                                          set<IEventSink *> &event_sinks);

private:
    string m_service_addr;
    string m_lib_addr;
    set<IEventSink *> m_watch_connect;

    ZMQTrans m_trans;
    Stage<ConfigHandler, EventQueue> m_stage;
    FsmMgr<ConfigFsm> m_fsm_mgr;

    map<string, WatchValue *> m_watch_value;
    map<string, WatchChild *> m_watch_child;

    ThreadLock m_client_lock;
    ThreadLock m_data_lock;
};

}//namespace common;
}//namespace zonda;

#endif