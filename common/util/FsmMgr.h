#ifndef ZONDA_COMMON_FSM_MANAGER_H
#define ZONDA_COMMON_FSM_MANAGER_H


#include <list>
#include <tr1/unordered_map>
#include "log/Logger.h"
#include "thread/ThreadLock.h"


namespace zonda
{
namespace common
{

using namespace std;
using namespace std::tr1;

template<class T> class FsmMgr;

template<class T>
class FsmBase
{
    friend class FsmMgr<T>;
public:
    int64_t get_id()
    {
        return m_id;
    }

    FsmMgr<T>* get_fsm_mgr()
    {
        return m_fsm_mgr;
    }
    
    //The subclass must to implement
    //to clear all the members so 
    //that this object can be reused.
    virtual void reset() = 0;
    void free()
    {
        m_fsm_mgr->free_fsm(m_id);
    }

protected:
    int64_t m_id;
    FsmMgr<T>* m_fsm_mgr;
    FsmBase(){};
    virtual ~FsmBase() {}
};   

//! This class manage all fsm.
                  
template<class T>
class FsmMgr
{
public:
    typedef unordered_map<int64_t, T*> FsmMap;
    typedef typename FsmMap::iterator FsmMapIterator;
    typedef std::list<T*> FsmList;
    typedef typename std::list<T*>::iterator FsmListIterator;
    ~FsmMgr()
    {
        free_all();
    }
    
    int init(int size)
    {
        m_lock.lock();
        T* p = NULL;
        for (int i=0; i<size; ++i)
        {   
            try
            {         
                p = new T();
            }
            catch (std::bad_alloc& ba)
            {
                this->free_all();
                m_lock.unlock();
                return -1;
            }
            m_free_list.push_back(p);
        }
        m_size = size;
        m_lock.unlock();
        return 0;
    }
    
    
    T* create_fsm(int64_t fsm_id)
    {
        m_lock.lock();
        FsmMapIterator it;
        it = m_map.find(fsm_id);
        if (it != m_map.end())
        {
            LOG_ERROR("fsm_id:" << fsm_id << " already exist");
            m_lock.unlock();
            return NULL;
        }
        
        if (m_free_list.size() == 0)
        {
            LOG_ERROR("No avaliable fsm now!");
            m_lock.unlock();
            return NULL;            
        }
        
        T* p = m_free_list.back();
        p->m_id = fsm_id;
        p->m_fsm_mgr = this;
        m_map[fsm_id] = p;
        m_free_list.pop_back();
        m_lock.unlock();
        return p;
    }
    
    T* create_fsm()
    {
        m_lock.lock();
        if (m_free_list.size() == 0)
        {
            LOG_ERROR("No any avaliable fsm now!");
            m_lock.unlock();
            return NULL;            
        }
        
        T* p = m_free_list.back();
        p->m_id = reinterpret_cast<int64_t>(p);
        p->m_fsm_mgr = this;
        m_map[p->m_id] = p;
        m_free_list.pop_back();
        //LOG_DEBUG("FSM create id: " << p->m_id);
        m_lock.unlock();       
        return p;        
    }
    
    T* get_fsm(int64_t fsm_id)
    {
        m_lock.lock();
        FsmMapIterator it;
        it = m_map.find(fsm_id);
        if (it == m_map.end())
        {
            //LOG_WARN("Not Found FSM  id: " << fsm_id);
            m_lock.unlock();
            return NULL;
        }
        m_lock.unlock();
        return it->second;
    }
    
    void free_fsm(int64_t fsm_id)
    {
        m_lock.lock();
        FsmMapIterator it;
        it = m_map.find(fsm_id);
        if (it != m_map.end())
        {
            it->second->reset();
            m_free_list.push_back(it->second);
            m_map.erase(fsm_id);
        }
        //LOG_DEBUG("FSM free id: " << fsm_id);
        m_lock.unlock();      
    }
    
    void free_fsm(T* fsm)
    {
        free_fsm(fsm->m_id);
    }
    
    int get_used_count()
    {
        m_lock.lock();
        int size = m_map.size();
        m_lock.unlock(); 
        return size;
    }
    
    int get_free_count()
    {
        m_lock.lock();
        int count = m_free_list.size();
        m_lock.unlock();
        return count;
    }
    
    int get_fsm_count()
    {
        return m_size;
    }
private:

    void free_all()
    {
        FsmMapIterator  map_it;
        for (map_it = m_map.begin(); map_it != m_map.end(); ++map_it)
        {
            delete map_it->second;

        }
        m_map.clear();
        
        FsmListIterator list_it;
        for (list_it = m_free_list.begin(); list_it != m_free_list.end(); ++list_it)
        {
            delete *list_it;
        }
        m_free_list.clear();      
    }
    FsmMap m_map;
    std::list<T*> m_free_list;
    int m_size;
    ThreadLock m_lock;
};

} //namespace common
} //namespace zonda


#endif //ZONDA_COMMON_FSM_MANAGER_H

