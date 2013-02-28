/**
 * @author fuping.yangfp@alibaba-inc.com
 */
#ifndef COMMON_CONFIG_CONFIG_H_
#define COMMON_CONFIG_CONFIG_H_

#include "util/Singleton.h"
#include "ZkConfigImpl.h"

namespace zonda
{
namespace common
{

typedef Singleton<ZkConfigImpl, ThreadLock> Config;

} //namespace common
} //namespace zonda
#endif /* COMMON_CONFIG_CONFIG_H_ */

