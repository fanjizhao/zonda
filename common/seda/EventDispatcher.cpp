#include <iostream>
#include <cstring>
#include "EventDispatcher.h"
#include "thread/Thread.h"
#include "log/Logger.h"

using namespace zonda::common;

EventDispatcher::EventDispatcher():
    m_internal_queues(NULL),
    m_internal_queue_num(0),
    m_queue_index(0)
{
    delete [] m_internal_queues;
    m_internal_queues = NULL;
}    

EventDispatcher::~EventDispatcher()
{
    
}
void EventDispatcher::enqueue(const IEvent* event)
{
    unsigned int i = ( __sync_fetch_and_add(&m_queue_index, 1) % m_internal_queue_num);
    //LOG_DEBUG("enqueue into i:" << i << ", event type:" << event->get_type());
    m_internal_queues[i]->enqueue(event);
}

size_t EventDispatcher::size()
{
    size_t s = 0;
    for (int i=0; i<m_internal_queue_num; ++i)
    {
        s += m_internal_queues[i]->size();
    }
    return s;
}

void EventDispatcher::set_internal_queues(IEventQueue** queues, int queue_num)
{
    if (m_internal_queues)
    {
        LOG_DEBUG("Reset the internal queues");
        delete [] m_internal_queues;
    }
    m_internal_queues = new IEventQueue*[queue_num];
    m_internal_queue_num = queue_num;
    
    for (int i=0; i<m_internal_queue_num; i++)
    {
        m_internal_queues[i] = queues[i];
    }
}



