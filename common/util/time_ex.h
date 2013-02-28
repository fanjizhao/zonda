#ifndef ZONDA_COMMON_TIME_EX_H_
#define ZONDA_COMMON_TIME_EX_H_


#include <sys/time.h>
namespace zonda
{
namespace common
{

//!Compare two struct timeval.
//!@retval 0 t1 == t2
//!@retval 1 t1 is later than t2
//!@retval -1 t1 is earlier than t2
int timeval_compare(const struct timeval &t1, const struct timeval &t2);

//!Return time1-time2
void timeval_subtract(const struct timeval &t1, const struct timeval &t2, struct timeval &result);

}//end namespace common
}//end namespace zonda

#endif //ZONDA_COMMON_TIME_EX_H_

