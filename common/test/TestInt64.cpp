/**
 * @author fuping.yangfp@alibaba-inc.com
 */
#include <stdint.h>
#include <sys/time.h>
#include <iostream>

using namespace std;

int main() {
	  long loop_count = 1000000000L;
	  
		time_t from = time(NULL);
		int total_int = 0;
		for (long i = 0; i < loop_count; ++i) {
			total_int++;
		}
	  cout << "total_int: " << total_int << "; lost:" << (time(NULL) - from) << endl;

		from = time(NULL);
		int32_t total_int32 = 0;
		for (long i = 0; i < loop_count; ++i) {
			total_int32++;
		}
	  cout << "total_int32: " << total_int32 << "; lost:" << (time(NULL) - from) << endl;

	  from = time(NULL);
		int64_t total_int64 = 0;
		for (long i = 0; i < loop_count; ++i) {
			total_int64++;
		}
	  cout << "total_int64: " << total_int64 << "; lost:" << (time(NULL) - from) << endl;
}
