/**
 * @author fuping.yangfp@alibaba-inc.com
 */
#include <cstring>
#include "ConfigChangeEvent.h"

using namespace zonda::common;

int ConfigChangeEvent::get_type() const {
    return EventType::CONFIG_CHANGE_EVENT;
}

ConfigChangeEvent *ConfigChangeEvent::clone() const {
    ConfigChangeEvent *e = new ConfigChangeEvent();
    e->m_change_type = m_change_type;
    e->m_path = m_path;
    if (m_value_len > 0) {
        e->m_value = new char[m_value_len];
        memcpy(e->m_value, m_value, m_value_len);
    }
    e->m_value_len = m_value_len;
    for (size_t idx = 0; idx != m_children.size(); ++idx) {
        e->m_children.push_back(m_children[idx]);
    }
            
    return e;
}



