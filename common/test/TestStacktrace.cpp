/**
 * To test util/stacktrace.h
 *
 * @author fuping.yangfp@alibaba-inc.com
 */
#include "../util/stacktrace.h"

using namespace std;

void test2(string a){
	print_stacktrace(a);
}

void test(string a) {
	test2(a);
}

int main(int argc, char **argv) {
		test("mytest");    
    return 0;
}
