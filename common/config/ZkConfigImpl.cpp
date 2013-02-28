
#include <unistd.h>
#include "ZkConfigImpl.h"
#include "thread/LockGuard.h"
#include "log/Logger.h"
#include "ConfigChangeEvent.h"

#define MAX_VALUE_LEN 1024*10
#define ENABLE_WATCH 1
#define DISABLE_WATCH 0
using namespace zonda::common;

void watcher_func(zhandle_t *zh, int type, int state, const char *path,
             void* context)
{
    LOG_DEBUG("type:" << type << ", state:" << state << ", path:" << path);
    
    ZkConfigImpl* config_impl = (ZkConfigImpl*)context;
    if (type == ZOO_SESSION_EVENT) 
    {
        if (state == ZOO_CONNECTED_STATE)
        {
            const clientid_t *id = zoo_client_id(zh);
            config_impl->finish_connection(id);                  
            return;
        }
    }
    else if (type == ZOO_DELETED_EVENT)
    {
        LOG_DEBUG(path << " has been deleted");
    }
    else if (type == ZOO_CHANGED_EVENT)
    {
        LOG_DEBUG(path << " value changed");
        config_impl->notify_value_change(path);
    }
    else if (type == ZOO_CHILD_EVENT)
    {
        LOG_DEBUG(path << " children changed");
        config_impl->notify_children_change(path);
    }
}

void ZkConfigImpl::finish_connection(const clientid_t* client_id)
{
    LockGuard guard(m_lock);
    m_zk_client_id = *client_id;
    m_connected = true;
    LOG_DEBUG("Got a new zookeeper client id:" << m_zk_client_id.client_id); 
}

void ZkConfigImpl::notify_children_change(const char* path)
{
    LockGuard guard(m_lock);
    
    map<string, WatchChildren>::iterator it;
    it = m_children_watcher.find(path);
    if (it == m_children_watcher.end())
    {
        LOG_INFO("path:" << path << " 's children changed, but nobody cares!");
        return;
    }
    
    struct String_vector strings;
    int r = zoo_get_children(m_zk_handle, path, ENABLE_WATCH, &strings);
    if (r != ZOK)
    {
        LOG_ERROR("Failed to get children of path:" << path << ", Error:" << zerror(r));
        return;
    }
    
    ConfigChangeEvent event;
    event.m_change_type = ConfigChangeEvent::CHILD;
    event.m_path = path;
    for (int i=0; i<strings.count; ++i)
    {
        string child;
        event.m_children.push_back(child);
        event.m_children[i] = strings.data[i];
        LOG_DEBUG("Got child of path:" << path << ", child:" << event.m_children[i]);
    }
    
    set<IEventSink *>::iterator set_it;
    for (set_it = it->second.event_sinks.begin(); set_it != it->second.event_sinks.end(); ++set_it)
    {
        ConfigChangeEvent* e = event.clone();
        IEventSink * sink = *set_it;
        LOG_DEBUG("sink=" << sink);
        sink->enqueue(e);
    }   

}


void ZkConfigImpl::notify_value_change(const char* path)
{
    LockGuard guard(m_lock);
    
    map<string, WatchValue>::iterator it;
    it = m_value_watcher.find(path);
    if (it == m_value_watcher.end())
    {
        LOG_INFO("path:" << path << " 's value changed, but nobody cares!");
        return;
    }
    
    char buff[MAX_VALUE_LEN];
    int len = MAX_VALUE_LEN-1;
    int r = zoo_get(m_zk_handle, path, ENABLE_WATCH, buff, &len, NULL);
    if (r != ZOK)
    {
        LOG_ERROR("Failed to get value of path:" << path << ", Error:" << zerror(r));
        return;
    }
    
    ConfigChangeEvent event;
    event.m_change_type = ConfigChangeEvent::CHANGED;
    event.m_path = path;
    event.m_value = new char[len+1];
    strcpy(event.m_value, buff);
    event.m_value_len = len;
    
    LOG_DEBUG("value:\n" << event.m_value);
    
    set<IEventSink *>::iterator set_it;
    for (set_it = it->second.event_sinks.begin(); set_it != it->second.event_sinks.end(); ++set_it)
    {
        ConfigChangeEvent* e = event.clone();
        (*set_it)->enqueue(e);
    }   
    
}


void ZkConfigImpl::notify_path_deleted(const char* path)
{
    LockGuard guard(m_lock);
    
    map<string, WatchExistence>::iterator it;
    it = m_existence_watcher.find(path);
    if (it == m_existence_watcher.end())
    {
        LOG_INFO("path:" << path << " has been deleted, but nobody cares!");
        return;
    }
    
    ConfigChangeEvent event;
    event.m_change_type = ConfigChangeEvent::DELETED;
    event.m_path = path;
    
    
    set<IEventSink *>::iterator set_it;
    for (set_it = it->second.event_sinks.begin(); set_it != it->second.event_sinks.end(); ++set_it)
    {
        ConfigChangeEvent* e = event.clone();
        (*set_it)->enqueue(e);
    }   
    
}


ZkConfigImpl::ZkConfigImpl():
    m_zk_handle(NULL),
    m_connected(false)
{
}

int ZkConfigImpl::init(const char *service_addr, int timeout) 
{
    const char* real_zk_addr = NULL;
    
    if (getenv(ZOOKEEPER_ADDR_LIST_ENV_VAR) != NULL)
    {
        real_zk_addr = getenv(ZOOKEEPER_ADDR_LIST_ENV_VAR);
    }
    else
    {
        if (service_addr == NULL || strlen(service_addr) == 0)
        {
            real_zk_addr = "127.0.0.1:2181";
        }
        else
        {
            real_zk_addr = service_addr;
        }
    }
    
    m_lock.lock();
    m_zk_handle = zookeeper_init(real_zk_addr, &watcher_func, 10000, 0, this, 0);
    if (m_zk_handle == NULL)
    {
        LOG_FATAL("Failed to connected to zookeeper: " << real_zk_addr
            << ", Error:" << strerror(errno));
        m_lock.unlock();
        return -1;
    }
    m_lock.unlock();
     
    for (int i=0; i<timeout; i+=100)
    {
        if (!m_connected)
        {
            //sleep 100 ms
            usleep(100*1000);
        }
        else
        {
            return 0;
        }
    }
    
    LOG_ERROR("Timed out for connecting to zookeeper:" << service_addr
        << ", timeout:" << timeout << " ms");
    return -1;
}



// flags can be NODE_EPHEMERAL or NODE_SEQUENCE or together
int ZkConfigImpl::create_path(const char *path, const char *value, int valuelen,
            int flags) 
{
    LockGuard guard(m_lock);
    int r = zoo_create(m_zk_handle, path, value, valuelen,  
        &ZOO_OPEN_ACL_UNSAFE, flags, NULL, 0);
    if (r == ZOK)
    {
        return 0;
    }
    LOG_ERROR("Failed to create path in zookeeper, Error:" << zerror(r));
    return -1;
}

int ZkConfigImpl::delete_path(const char *path) 
{
    LockGuard guard(m_lock);
    int r = zoo_delete(m_zk_handle, path, -1);
    if (r == ZOK)
    {
        return 0;
    }
    LOG_ERROR("Failed to create path in zookeeper, Error:" << zerror(r));
    return -1;
}

// CONFIG_CHANGE_EVENT event will put into event_sink if event_sink is not NULL
int ZkConfigImpl::exists_path(const char *path, bool &existed,
            IEventSink *event_sink, bool remove_watch) 
{
    LockGuard guard(m_lock);
    map<string, WatchExistence>::iterator it = m_existence_watcher.find(path);
    if (remove_watch)
    {        
        if (it != m_existence_watcher.end())
        {
            it->second.event_sinks.erase(event_sink);
        }
    }
    else
    {
        if (it == m_existence_watcher.end())
        {
            WatchExistence we;
            string s = path;
            m_existence_watcher.insert(make_pair(s,we));
            it = m_existence_watcher.find(s);
        }

        it->second.event_sinks.insert(event_sink);
        
    }
    
    int r = zoo_exists(m_zk_handle, path, ENABLE_WATCH, NULL);
    if (r == ZOK)
    {
        return 0;
    }
    LOG_ERROR("Failed to check the existence of path:" << path << ", Error:" << zerror(r));
    return -1;
}

// value with length value_len need be created and freed by programmer outside
// CONFIG_CHANGE_EVENT event will put into event_sink if event_sink is not NULL
int ZkConfigImpl::get_value(const char *path, char *value, int *value_len,
          IEventSink *event_sink, bool remove_watch) 
{
    LockGuard guard(m_lock);
    map<string, WatchValue>::iterator it = m_value_watcher.find(path);
    if (remove_watch)
    {        
        if (it != m_value_watcher.end())
        {
            it->second.event_sinks.erase(event_sink);
        }
    }
    else
    {
        if (it == m_value_watcher.end())
        {
            WatchValue wv;
            string s = path;
            m_value_watcher.insert(make_pair(s,wv));
            it = m_value_watcher.find(s);
        }

        it->second.event_sinks.insert(event_sink);
        
    }
    
    int r = zoo_get(m_zk_handle, path, ENABLE_WATCH, value, value_len, NULL);
    if (r == ZOK)
    {
        return 0;
    }
    LOG_ERROR("Failed to get value of path:" << path << ", Error:" << zerror(r));
    return -1;
}

int ZkConfigImpl::set_value(const char *path, const char *value, int value_len) 
{
    LockGuard guard(m_lock);
    int r = zoo_set(m_zk_handle, path, value, value_len, -1);
    if (r == ZOK)
    {
        return 0;
    }
    LOG_ERROR("Failed to set value of path:" << path << ", Error:" << zerror(r));
    return -1;    
}

// CONFIG_CHANGE_EVENT event will put into event_sink if event_sink is not NULL
int ZkConfigImpl::get_children(const char *path, vector<string> &children,
             IEventSink *event_sink, bool remove_watch) 
{
    
    LockGuard guard(m_lock);
    map<string, WatchChildren>::iterator it = m_children_watcher.find(path);
    if (remove_watch)
    {        
        if (it != m_children_watcher.end())
        {
            it->second.event_sinks.erase(event_sink);
        }
    }
    else
    {
        if (it == m_children_watcher.end())
        {
            WatchChildren wc;
            string s = path;
            m_children_watcher.insert(make_pair(s,wc));
            it = m_children_watcher.find(s);
        }

        it->second.event_sinks.insert(event_sink);
        LOG_DEBUG("insert watch sink:" << event_sink << ", for path:" << it->first);
    }
    
    struct String_vector strings;
    int r = zoo_get_children(m_zk_handle, path, ENABLE_WATCH, &strings);
    if (r != ZOK)
    {
        LOG_ERROR("Failed to get value of path:" << path << ", Error:" << zerror(r));
        return -1;
    }

    for (int i=0; i<strings.count; ++i)
    {
        string child;
        children.push_back(child);
        children[i].assign(strings.data[i]);
    }
        
    return 0;
}

/* C++ style versions start */
int ZkConfigImpl::create_path(const string &path, const string &value, int flags) 
{
    return create_path(path.c_str(), value.c_str(), value.size() + 1, flags);
}

int ZkConfigImpl::delete_path(const string &path) 
{
    return delete_path(path.c_str());
}

int ZkConfigImpl::exists_path(const string &path, bool &existed,
            IEventSink *event_sink, bool remove_watch) 
{
    return exists_path(path.c_str(), existed, event_sink, remove_watch);
}


int ZkConfigImpl::get_value(const string &path, string &value,
          IEventSink *event_sink, bool remove_watch) 
{
    char buff[MAX_VALUE_LEN];
    int len = MAX_VALUE_LEN;
    int rtn = get_value(path.c_str(), buff, &len, event_sink, remove_watch);
    if (rtn) 
    {
        return rtn;
    } 
    else 
    {
        value.assign(buff, len);
        return 0;
    }
}

int ZkConfigImpl::set_value(const string &path, const string &value) 
{
    return set_value(path.c_str(), value.c_str(), value.size() + 1);
}

int ZkConfigImpl::get_children(const string &path, vector<string> &children,
             IEventSink *event_sink, bool remove_watch) 
{
    return get_children(path.c_str(), children, event_sink, remove_watch);
}