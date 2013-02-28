/**
 * @author fuping.yangfp@alibaba-inc.com
 */
#ifndef CONFIG_ALARMID_H_
#define CONFIG_ALARMID_H_

namespace zonda {
namespace common {

class AlarmId {
public:
    static const int TEST;
    static const int MONITOR_COUNTER_FAILURE;
    static const int MONITOR_LOAD;
};

} //namespace common
} //namespace zonda
#endif /* CONFIG_ALARMID_H_ */

