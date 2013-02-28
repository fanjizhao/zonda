/**
 * @author fuping.yangfp@alibaba-inc.com
 */
#ifndef COMMON_CONFIG_CONFIGCHANGEEVENT_H_
#define COMMON_CONFIG_CONFIGCHANGEEVENT_H_

#include <string>
#include <vector>
#include "seda/IEvent.h"
#include "seda/EventType.h"

namespace zonda {
namespace common {
using namespace std;

class ConfigChangeEvent : public IEvent {
public:
    ConfigChangeEvent()
        : m_value(NULL), m_value_len(0) {};
    virtual ~ConfigChangeEvent() {
        delete[] m_value;
    };
    virtual int get_type() const ;
    virtual ConfigChangeEvent* clone() const ;

public:
    enum ChangeType {
        // Connected to ZK
        CONNECTED,
        // Disconnected to ZK
        DISCONNECTED,
        // These watches are set using exists_path for non-existent nodes.
        CREATED,
        // These watches are set using exists_path and get_value.
        DELETED,
        // These watches are set using get_value.
        // Can NOT set using exists_path after testing. (zookeeper doc has error.)
        CHANGED,
        // These watches are set using get_children.
        CHILD
    };

    ChangeType m_change_type;
    string m_path;
    char *m_value; // only available for change_type CREATED and CHANGED
    int m_value_len; // only available for change_type CREATED and CHANGED
    vector<string> m_children; // only available for change_type CHILD
};

}//namespace common
}//namespace zonda

#endif //COMMON_CONFIG_CONFIGCHANGEEVENT_H_

