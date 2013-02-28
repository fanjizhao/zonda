
#include "time_ex.h"
namespace zonda
{
namespace common
{

int timeval_compare(const struct timeval &t1, const struct timeval &t2)
{
	if (t1.tv_sec > t2.tv_sec) return 1;
	else if (t1.tv_sec < t2.tv_sec) return -1;
	else
	{
		if (t1.tv_usec > t2.tv_usec) return 1;
		else if (t1.tv_usec < t2.tv_usec) return -1;
		else return 0;
	}
}

void timeval_subtract(const struct timeval &t1, const struct timeval &t2, struct timeval &result)
{
	result.tv_sec = t1.tv_sec - t2.tv_sec;
	result.tv_usec = t1.tv_usec - t2.tv_usec;
	
}


}//end namespace common
}//end namespace zonda

