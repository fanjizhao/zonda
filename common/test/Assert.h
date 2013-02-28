/**
 * @author fuping.yangfp@alibaba-inc.com
 */
#ifndef COMMON_TEST_ASSERT_H_
#define COMMON_TEST_ASSERT_H_

#include <cstdio>
#include <cstring>
static const char *ASSERT_MSG_PREFIX = ">>===========";
bool test_failed = false;

void assert_d(const char *msg, long expected, long actual) {
    if (expected != actual) {
        test_failed = true;
        printf(ASSERT_MSG_PREFIX);
        printf("%s", msg);
        printf(": Expected(%ld) != Actual(%ld)\n", expected, actual);
    }
}

void assert_s(const char *msg, const char *expected, const char *actual) {
    if (strcmp(expected, actual)) {
        test_failed = true;
        printf(ASSERT_MSG_PREFIX);
        printf("%s", msg);
        printf(": Expected(%s) != Actual(%s)\n", expected, actual);
    }
}

void test_done() {
    printf(ASSERT_MSG_PREFIX);
    if (test_failed) {
        printf("Test Failure\n");
    } else {
        printf("Test Success\n");
    }
}

#endif /* COMMON_TEST_ASSERT_H_ */

