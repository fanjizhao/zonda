/**
 * @author fuping.yangfp@alibaba-inc.com
 */
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include "ConfigTransfer.h"
#include "ConfigHandler.h"
#include "ZondaConfigImpl.h"
#include "log/Logger.h"

string zonda_conf_lib_file;
void delete_zonda_conf_lib_file() {
    remove(zonda_conf_lib_file.c_str());
}
void handle_zonda_signal(int signo) {
    delete_zonda_conf_lib_file();
    printf("Received signo %d\n", signo);
    exit(-1);
}


using namespace std;
using namespace zonda::common;


static const int WAIT_TIMEOUT_MILLISECONDS = 10000;

int ZondaConfigImpl::init(const char *service_addr) {
    if (!m_lib_addr.empty()) {
        // have already been initialized
        return 0;
    }
    if (service_addr == NULL || !strcmp(service_addr, "")) {
        m_service_addr = "/tmp/conf_service";
    } else {
        m_service_addr = service_addr;
    }
    char lib_addr[MAX_LIB_ADDR_LEN];
    snprintf(lib_addr, MAX_LIB_ADDR_LEN, "/tmp/conf_lib_%d", getpid());
    m_lib_addr = lib_addr;

    zonda_conf_lib_file = m_lib_addr;
    atexit(delete_zonda_conf_lib_file);

    struct sigaction act, oldact;
    act.sa_handler = handle_zonda_signal;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    int rtn = sigaction(SIGHUP, &act, &oldact);
    if (rtn < 0) {
        LOG_FATAL("Wrong to sigaction, rtn=" << rtn);
        return -1;
    }
    rtn = sigaction(SIGINT, &act, &oldact);
    if (rtn < 0) {
        LOG_FATAL("Wrong to sigaction, rtn=" << rtn);
        return -1;
    }
    rtn = sigaction(SIGQUIT, &act, &oldact);
    if (rtn < 0) {
        LOG_FATAL("Wrong to sigaction, rtn=" << rtn);
        return -1;
    }
    rtn = sigaction(SIGTERM, &act, &oldact);
    if (rtn < 0) {
        LOG_FATAL("Wrong to sigaction, rtn=" << rtn);
        return -1;
    }
    rtn = sigaction(SIGUSR1, &act, &oldact);
    if (rtn < 0) {
        LOG_FATAL("Wrong to sigaction, rtn=" << rtn);
        return -1;
    }

    rtn = m_stage.init(this);
    if (rtn) {
        LOG_FATAL("Wrong to init stage, rtn=" << rtn);
        return -1;
    }
    rtn = m_stage.start();
    if (rtn) {
        LOG_FATAL("Wrong to start stage, rtn=" << rtn);
        return -1;
    }

    rtn = m_fsm_mgr.init(1000);
    if (rtn) {
        LOG_FATAL("Failed to init fsm mgr, rtn=" << rtn);
    }

    TransParm param;
    param.ipc_addr_list.push_back(m_lib_addr);
    param.recv_sink = m_stage.get_sink();
    param.auto_listen_tcp = false;
    rtn = m_trans.init(&param);
    if (rtn) {
        LOG_FATAL("Wrong to init ZMQTrans, rtn=" << rtn);
        return -1;
    }
    rtn = m_trans.start();
    if (rtn) {
        LOG_FATAL("Wrong to start ZMQTrans, rtn=" << rtn);
        return -1;
    }

    rtn = register_config_lib();
    if (rtn) {
        LOG_FATAL("Wrong to init register config lib, rtn=" << rtn);
        return -1;
    }
    
    LOG_INFO("Config has successfully initialized.");
    return 0;
}

int ZondaConfigImpl::register_config_lib() {
    LOG_INFO("register_config_lib ...");
    m_client_lock.lock();
    int rtn = -1;
    ConfigFsm *fsm = m_fsm_mgr.create_fsm();
    if (handle_req(HeadData::REGISTER_CONFIG_LIB, "", false, 0, "", 1, fsm, NULL)) {
        LOG_ERROR("Fail to call handle_req for register config lib");
    } else {
        if (fsm->cond.time_wait(WAIT_TIMEOUT_MILLISECONDS)) {
            LOG_ERROR("Timeout wait to receive response from ConfigService for register config lib");
        } else {
            rtn = fsm->response.ret_code;
        }
    }
    fsm->free();
    m_client_lock.unlock();
    return rtn;
}

void ZondaConfigImpl::add_watch_connect(IEventSink *event_sink) {
    if (event_sink) {
        m_data_lock.lock();
        m_watch_connect.insert(event_sink);
        m_data_lock.unlock();
    }
}

void ZondaConfigImpl::remove_watch_connect(IEventSink *event_sink) {
    if (event_sink) {
        m_data_lock.lock();
        m_watch_connect.erase(event_sink);
        m_data_lock.unlock();
    }
}

int ZondaConfigImpl::create_path(const char *path, const char *value, int valuelen,
                            int flags) {
    LOG_INFO("create_path on path " << path);
    m_client_lock.lock();
    int rtn = -1;
    ConfigFsm *fsm = m_fsm_mgr.create_fsm();
    if (handle_req(HeadData::CREATE, path, false, flags, value, valuelen, fsm, NULL)) {
        LOG_ERROR("Fail to call handle_req on path " << path);
    } else {
        if (fsm->cond.time_wait(WAIT_TIMEOUT_MILLISECONDS)) {
            LOG_ERROR("Timeout wait to receive response from ConfigService on path " << path);
        } else {
            rtn = fsm->response.ret_code;
        }
    }
    fsm->free();
    m_client_lock.unlock();
    return rtn;
}

int ZondaConfigImpl::delete_path(const char *path) {
    LOG_INFO("delete_path on path " << path);
    m_client_lock.lock();
    int rtn = -1;
    ConfigFsm *fsm = m_fsm_mgr.create_fsm();
    if (handle_req(HeadData::DELETE, path, false, false, "", 1, fsm, NULL)) {
        LOG_ERROR("Fail to call handle_req on path " << path);
    } else {
        if (fsm->cond.time_wait(WAIT_TIMEOUT_MILLISECONDS)) {
            LOG_ERROR("Timeout wait to receive response from ConfigService on path " << path);
        } else {
            if (fsm->response.ret_code) {
                rtn = fsm->response.ret_code;
            } else {
                string path_s = path;
                remove_from_cache(path_s);
                rtn = fsm->response.ret_code;
            }
        }
    }
    fsm->free();
    m_client_lock.unlock();
    return rtn;
}

int ZondaConfigImpl::exists_path(const char *path, bool &existed,
                            IEventSink *event_sink, bool remove_watch) {
    LOG_INFO("exists_path on path " << path << "; event_sink " << event_sink);
    m_client_lock.lock();
    int rtn = -1;

    WatchValue *watch_value = NULL;
    get_from_cache(watch_value, false, path);
    if (watch_value != NULL) {
        existed = watch_value->existed;
        if (event_sink != NULL) {
            if (remove_watch) {
                remove_from_cache(path, event_sink, false);
            } else {
                add_to_cache(path, event_sink, false);
            }
        }

        m_client_lock.unlock();
        return 0;
    }

    ConfigFsm *fsm = m_fsm_mgr.create_fsm();
    if (handle_req(HeadData::EXISTS, path, !remove_watch, false, "", 1, fsm, NULL)) {
        LOG_ERROR("Fail to call handle_req on path " << path);
    } else {
        if (fsm->cond.time_wait(WAIT_TIMEOUT_MILLISECONDS)) {
            LOG_ERROR("Timeout wait to receive response from ConfigService on path " << path);
        } else {
            existed = !(fsm->response.ret_code);

            if (event_sink != NULL && !remove_watch) {
                update_to_cache(path, true, NULL, 0, NULL, event_sink);
            }
            rtn = 0;
        }
    }
    fsm->free();
    m_client_lock.unlock();
    return rtn;
}

int ZondaConfigImpl::get_value(const char *path, char *value, int *value_len,
                          IEventSink *event_sink, bool remove_watch) {
    LOG_INFO("get_value on path " << path << "; event_sink " << event_sink);
    m_client_lock.lock();
    int rtn = -1;

    WatchValue *watch_value = NULL;
    get_from_cache(watch_value, true, path);
    if (watch_value != NULL) {
        if (watch_value->value_len <= *value_len) {
            memcpy(value, watch_value->value, watch_value->value_len);
            *value_len = watch_value->value_len;

            if (event_sink != NULL) {
                if (remove_watch) {
                    remove_from_cache(path, event_sink, false);
                } else {
                    add_to_cache(path, event_sink, false);
                }
            }
            m_client_lock.unlock();
            return 0;
        } else {
            LOG_ERROR("Too small value buffer size: " << *value_len << " < " << watch_value->value_len);
            m_client_lock.unlock();
            return -1;
        }
    }

    ConfigFsm *fsm = m_fsm_mgr.create_fsm();
    if (fsm == NULL) {
        LOG_ERROR("Failed to create fsm.");
        m_client_lock.unlock();
        return -1;
    }
    if (handle_req(HeadData::GET, path, !remove_watch, false, "", 1, fsm, NULL)) {
        LOG_ERROR("Fail to call handle_req on path " << path);
    } else {
        if (fsm->cond.time_wait(WAIT_TIMEOUT_MILLISECONDS)) {
            LOG_ERROR("Timeout wait to receive response from ConfigService on path " << path);
        } else {
            if (fsm->response.ret_code) {
                rtn = fsm->response.ret_code;
            } else {
                if (fsm->response.value_len <= *value_len) {
                    memcpy(value, fsm->response.value, fsm->response.value_len);
                    *value_len = fsm->response.value_len;

                    if (event_sink != NULL && !remove_watch) {
                        update_to_cache(path, true, value, *value_len, NULL, event_sink);
                    }
                    rtn = 0;
                } else {
                    LOG_ERROR("Too small value buffer size: " << *value_len << " < " << fsm->response.value_len);
                }
            }
        }
    }
    fsm->free();
    m_client_lock.unlock();
    return rtn;
}

int ZondaConfigImpl::set_value(const char *path, const char *value, int value_len) {
    LOG_INFO("set_value on path " << path);
    m_client_lock.lock();
    int rtn = -1;
    ConfigFsm *fsm = m_fsm_mgr.create_fsm();

    if (handle_req(HeadData::SET, path, false, false, value, value_len, fsm, NULL)) {
        LOG_ERROR("Fail to call handle_req on path " << path);
    } else {
        if (fsm->cond.time_wait(WAIT_TIMEOUT_MILLISECONDS)) {
            LOG_ERROR("Timeout wait to receive response from ConfigService on path " << path);
        } else {
            rtn = fsm->response.ret_code;
        }
    }
    fsm->free();
    m_client_lock.unlock();
    return rtn;
}

int ZondaConfigImpl::get_children(const char *path, vector<string> &children,
                             IEventSink *event_sink, bool remove_watch) {
    LOG_INFO("get_children on path " << path << "; event_sink " << event_sink);
    m_client_lock.lock();
    int rtn = -1;

    WatchChild *watch_child = NULL;
    get_from_cache(watch_child, path);
    if (watch_child != NULL) {
        children = watch_child->children;

        if (event_sink != NULL) {
            if (remove_watch) {
                remove_from_cache(path, event_sink, true);
            } else {
                add_to_cache(path, event_sink, true);
            }
        }
        m_client_lock.unlock();
        return 0;
    }

    ConfigFsm *fsm = m_fsm_mgr.create_fsm();
    if (handle_req(HeadData::GET_CHILDREN, path, !remove_watch, false, "", 1, fsm, NULL)) {
        LOG_ERROR("Fail to call handle_req on path " << path);
    } else {
        if (fsm->cond.time_wait(WAIT_TIMEOUT_MILLISECONDS)) {
            LOG_ERROR("Timeout wait to receive response from ConfigService on path " << path);
        } else {
            if (fsm->response.ret_code) {
                rtn = fsm->response.ret_code;
            } else {
                children = fsm->children;
                if (event_sink != NULL && !remove_watch) {
                    update_to_cache(path, true, NULL, 0, &(fsm->children), event_sink);
                }
                rtn = 0;
            }
        }
    }
    fsm->free();
    m_client_lock.unlock();
    return rtn;
}

/* C++ style versions start */
int ZondaConfigImpl::create_path(const string &path, const string &value, int flags) {
    return create_path(path.c_str(), value.c_str(), value.size() + 1, flags);
}
int ZondaConfigImpl::delete_path(const string &path) {
    return delete_path(path.c_str());
}
int ZondaConfigImpl::exists_path(const string &path, bool &existed,
                            IEventSink *event_sink, bool remove_watch) {
    return exists_path(path.c_str(), existed, event_sink, remove_watch);
}
int ZondaConfigImpl::get_value(const string &path, string &value,
                          IEventSink *event_sink, bool remove_watch) {
    char buff[MAX_VALUE_LEN];
    int len = MAX_VALUE_LEN;
    int rtn = get_value(path.c_str(), buff, &len, event_sink, remove_watch);
    if (rtn) {
        return rtn;
    } else {
        value.assign(buff, len);
        return 0;
    }
}
int ZondaConfigImpl::set_value(const string &path, const string &value) {
    return set_value(path.c_str(), value.c_str(), value.size() + 1);
}
int ZondaConfigImpl::get_children(const string &path, vector<string> &children,
                             IEventSink *event_sink, bool remove_watch) {
    return get_children(path.c_str(), children, event_sink, remove_watch);
}

/* Private implementation start */
int ZondaConfigImpl::handle_req(int op_type, const char *path, bool watch,
                           int flags, const char *value, int value_len,
                           ConfigFsm *fsm, IEventSink *event_sink) {
    if ((int) strlen(path) > MAX_PATH_LEN) {
        LOG_ERROR("Too long path: " << path);
        return -1;
    }
    if (fsm == NULL) {
        LOG_ERROR("ConfigFsm can not be NULL.");
        return -1;
    }
    fsm->request.head.op_type = op_type;
    fsm->request.head.fsm_id = fsm->get_id();
    fsm->event_sink = event_sink;
    strcpy(fsm->request.lib_addr, m_lib_addr.c_str());
    strcpy(fsm->request.path, path);
    fsm->request.watch = watch;
    fsm->request.flags = flags;
    fsm->request.value_len = value_len;
    memcpy(fsm->request.value, value, value_len);

    char *buffer = new char[fsm->request.size()];
    fsm->request.serialize(buffer);

    m_trans.send_data(1, (uint8_t *)buffer, fsm->request.size(), m_service_addr.c_str());
    LOG_INFO("Config Lib sent to " << m_service_addr.c_str()
             << " with len " << fsm->request.size()
             << " for lib fsm_id " << fsm->request.head.fsm_id
             << " , op_type " << fsm->request.head.get_op_type_desc()
             << "(" << fsm->request.head.op_type << ")");
    return 0;
}

void ZondaConfigImpl::handle_res(const char *buff, int buff_len, int64_t fsm_id) {
    ConfigFsm *fsm = m_fsm_mgr.get_fsm(fsm_id);
    if (fsm == NULL) {
        LOG_ERROR("Get wrong or timeout fsm_id " << fsm_id);
        return;
    }
    fsm->response.deserialize(buff);

    if (fsm->response.head.op_type == HeadData::GET_CHILDREN_RES) {
        char *source = fsm->response.value;
        LOG_DEBUG("Find child num: " << fsm->response.strings_count);
        for (int i = 0; i < fsm->response.strings_count; i++) {
            LOG_DEBUG("Find child " << i << ": " << source);
            fsm->children.push_back(source);
            source += strlen(source) + 1;
        }
    }

    if (fsm->internal) {
        ConfigChangeEvent *e = new ConfigChangeEvent();
        if (fsm->internal_op == HeadData::NOTIFY_CREATED) {
            e->m_change_type = ConfigChangeEvent::CREATED;
        } else if (fsm->internal_op == HeadData::NOTIFY_CHANGED) {
            e->m_change_type = ConfigChangeEvent::CHANGED;
        } else if (fsm->internal_op == HeadData::NOTIFY_CHILD) {
            e->m_change_type = ConfigChangeEvent::CHILD;
        } else {
            LOG_FATAL("Get wrong fsm->internal_op " << fsm->internal_op);
        }

        e->m_path = fsm->response.path;
        string path_s = fsm->response.path;
        if (e->m_change_type == ConfigChangeEvent::CHILD) {
            e->m_children = fsm->children;

            set<IEventSink *> event_sinks;
            get_event_sinks_from_watch_child(path_s, event_sinks);

            LOG_INFO("Notify change to " << event_sinks.size()
                     << " event_sinks. "
                     << "path=" << e->m_path
                     << ", children.size()=" << e->m_children.size());
            notify_watch(event_sinks, e);
        } else {
            e->m_value_len = fsm->response.value_len;
            e->m_value = new char[e->m_value_len];
            memcpy(e->m_value, fsm->response.value, fsm->response.value_len);

            set<IEventSink *> event_sinks;
            get_event_sinks_from_watch_value(path_s, event_sinks);

            LOG_INFO("Notify change to " << event_sinks.size()
                     << " event_sinks. "
                     << "path=" << e->m_path
                     << ", value_len=" << e->m_value_len
                     << ", value=" << e->m_value);
            notify_watch(event_sinks, e);
        }
        fsm->free();
    } else {
        fsm->cond.signal();
    }
}

void ZondaConfigImpl::notify_data(const char *buff, int buff_len) {
    LOG_INFO("notify_data ...");

    ResponseData response;
    response.deserialize(buff);

    string path_s = response.path;
    if (response.head.op_type == HeadData::NOTIFY_CHILD) {
        if (!has_watch_child_cache(path_s)) {
            LOG_WARN("Child notification discarded on no watch path " << path_s);
            return;
        }
    } else if (response.head.op_type == HeadData::NOTIFY_CREATED
        || response.head.op_type == HeadData::NOTIFY_DELETED
        || response.head.op_type == HeadData::NOTIFY_CHANGED) {
        if (!has_watch_value_cache(path_s)) {
            LOG_WARN("Value notification discarded on no watch path " << path_s);
            return;
        }
    }

    if (response.head.op_type == HeadData::NOTIFY_CONNECTED
        || response.head.op_type == HeadData::NOTIFY_DISCONNECTED) {
        ConfigChangeEvent *e = new ConfigChangeEvent();
        if (response.head.op_type == HeadData::NOTIFY_CONNECTED) {
            e->m_change_type = ConfigChangeEvent::CONNECTED;
        } else {
            e->m_change_type = ConfigChangeEvent::DISCONNECTED;
        }

        set<IEventSink *> event_sinks;
        get_event_sinks_from_watch_connect(event_sinks);
        notify_watch(event_sinks, e);
    } else if (response.head.op_type == HeadData::NOTIFY_DELETED) {
        ConfigChangeEvent *e = new ConfigChangeEvent();
        e->m_path = response.path;
        e->m_change_type = ConfigChangeEvent::DELETED;

        set<IEventSink *> event_sinks;
        get_event_sinks_from_watch_value(e->m_path, event_sinks);
        notify_watch(event_sinks, e);
    } else {
        get_data_async(response.path, response.head.op_type);
    }
}

void ZondaConfigImpl::get_data_async(const char *path, int internal_op) {
    ConfigFsm *fsm = m_fsm_mgr.create_fsm();
    fsm->internal = true;
    fsm->internal_op = internal_op;

    int new_op_type = HeadData::GET;
    if (internal_op == HeadData::NOTIFY_CHILD) {
        new_op_type = HeadData::GET_CHILDREN;
    }
    if (handle_req(new_op_type, path, true, 0, "", 1, fsm, NULL)) {
        LOG_ERROR("Fail to call handle_req on path " << path);
        fsm->free();
    }
}

void ZondaConfigImpl::notify_watch(set<IEventSink *> &event_sinks, ConfigChangeEvent *e) {
    if (!event_sinks.empty()) {
        set<IEventSink *>::const_iterator iter = event_sinks.begin();
        IEventSink *event_sink = *iter;
        ++iter;
        for (; iter != event_sinks.end(); ++iter) {
            (*iter)->enqueue(e->clone());
            LOG_DEBUG("Enqueue to event_sink: " << *iter);
        }
        event_sink->enqueue(e);
        LOG_DEBUG("Enqueue to event_sink: " << event_sink);
    }
}

bool ZondaConfigImpl::has_watch_child_cache(const string &path_s) {
    m_data_lock.lock();
    bool has_cache = m_watch_child.find(path_s) != m_watch_child.end();
    m_data_lock.unlock();
    return has_cache;
}

bool ZondaConfigImpl::has_watch_value_cache(const string &path_s) {
    m_data_lock.lock();
    bool has_cache = m_watch_value.find(path_s) != m_watch_value.end();
    m_data_lock.unlock();
    return has_cache;
}

void ZondaConfigImpl::get_event_sinks_from_watch_connect(set<IEventSink *> &event_sinks) {
    m_data_lock.lock();
    event_sinks = m_watch_connect;
    m_data_lock.unlock();
}

void ZondaConfigImpl::get_event_sinks_from_watch_child(const string &path_s,
        set<IEventSink *> &event_sinks) {
    m_data_lock.lock();
    event_sinks = m_watch_child[path_s]->event_sinks;
    m_data_lock.unlock();
}

void ZondaConfigImpl::get_event_sinks_from_watch_value(const string &path_s,
        set<IEventSink *> &event_sinks) {
    m_data_lock.lock();
    event_sinks = m_watch_value[path_s]->event_sinks;
    m_data_lock.unlock();
}

void ZondaConfigImpl::get_from_cache(WatchValue *&watch_value, bool need_value,
                                const string &path_s) {
    m_data_lock.lock();
    if (m_watch_value.find(path_s) != m_watch_value.end()) {
        if (!need_value || m_watch_value[path_s]->value != NULL) {
            watch_value = m_watch_value[path_s];
            LOG_INFO("Found and return value cache for path " << path_s);
        }
    }
    m_data_lock.unlock();
}

void ZondaConfigImpl::get_from_cache(WatchChild *&watch_child, const string &path_s) {
    m_data_lock.lock();
    if (m_watch_child.find(path_s) != m_watch_child.end()) {
        LOG_INFO("Found and return child cache for path " << path_s);
        watch_child = m_watch_child[path_s];
    }
    m_data_lock.unlock();
}

void ZondaConfigImpl::update_to_cache(const string &path_s, const bool existed,
                                 const char *value, int value_len,
                                 const vector<string> *children, IEventSink *event_sink) {
    m_data_lock.lock();
    if (children != NULL) {
        WatchChild *watch_child;
        map<string, WatchChild *>::iterator it = m_watch_child.find(path_s);
        if (it != m_watch_child.end()) {
            watch_child = (WatchChild *) it->second;
            LOG_INFO("Update cached child for watched path " << path_s);
        } else {
            watch_child = new WatchChild();
            m_watch_child[path_s] = watch_child;
            LOG_INFO("Insert cached child for watched path " << path_s);
        }
        watch_child->children = *children;
        watch_child->event_sinks.insert(event_sink);
    } else {
        WatchValue *watch_value;
        map<string, WatchValue *>::iterator it = m_watch_value.find(path_s);
        if (it != m_watch_value.end()) {
            watch_value = (WatchValue *) it->second;
            LOG_INFO("Update cached value for watched path " << path_s);
        } else {
            watch_value = new WatchValue();
            m_watch_value[path_s] = watch_value;
            LOG_INFO("Insert cached value for watched path " << path_s);
        }
        watch_value->existed = existed;
        if (value_len != 0) {
            watch_value->value_len = value_len;
            delete[] watch_value->value;
            watch_value->value = new char[value_len];
            memcpy(watch_value->value, value, value_len);
        }
        watch_value->event_sinks.insert(event_sink);
    }
    m_data_lock.unlock();
}

void ZondaConfigImpl::add_to_cache(const string &path_s, IEventSink *event_sink,
                              bool child_cache) {
    m_data_lock.lock();
    if (child_cache) {
        map<string, WatchChild *>::iterator it = m_watch_child.find(path_s);
        if (it == m_watch_child.end()) {
            LOG_ERROR("Child cache not found for watched path " << path_s);
            m_data_lock.unlock();
            return;
        }
        WatchChild *watch_child = (WatchChild *) it->second;
        watch_child->event_sinks.insert(event_sink);
        LOG_INFO("Add event sink to child cache for watched path " << path_s);
    } else {
        map<string, WatchValue *>::iterator it = m_watch_value.find(path_s);
        if (it == m_watch_value.end()) {
            LOG_ERROR("Value cache not found for watched path " << path_s);
            m_data_lock.unlock();
            return;
        }
        WatchValue *watch_value = (WatchValue *) it->second;
        watch_value->event_sinks.insert(event_sink);
        LOG_INFO("Add event sink to value cache for watched path " << path_s);
    }
    m_data_lock.unlock();
}

void ZondaConfigImpl::remove_from_cache(const string &path_s, IEventSink *event_sink,
                                   bool child_cache) {
    m_data_lock.lock();
    if (child_cache) {
        if (m_watch_child.find(path_s) != m_watch_child.end()) {
            WatchChild *watch_child = m_watch_child[path_s];
            watch_child->event_sinks.erase(event_sink);
            if (watch_child->event_sinks.empty()) {
                m_watch_child.erase(path_s);
                delete watch_child;
            }
            LOG_INFO("Remove cached child for watched path " << path_s);
        }
    } else {
        if (m_watch_value.find(path_s) != m_watch_value.end()) {
            WatchValue *watch_value = m_watch_value[path_s];
            watch_value->event_sinks.erase(event_sink);
            if (watch_value->event_sinks.empty()) {
                m_watch_value.erase(path_s);
                delete watch_value;
            }
            LOG_INFO("Remove cached value for watched path " << path_s);
        }
    }
    m_data_lock.unlock();
}

void ZondaConfigImpl::remove_from_cache(const string &path_s) {
    m_data_lock.lock();
    if (m_watch_child.find(path_s) != m_watch_child.end()) {
        WatchChild *watch_child = m_watch_child[path_s];
        m_watch_child.erase(path_s);
        delete watch_child;
    }
    if (m_watch_value.find(path_s) != m_watch_value.end()) {
        WatchValue *watch_value = m_watch_value[path_s];
        m_watch_value.erase(path_s);
        delete watch_value;
    }
    m_data_lock.unlock();
}



