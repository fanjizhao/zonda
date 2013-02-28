/**
 * @author fuping.yangfp@alibaba-inc.com
 */
#include "sys/time.h"
#include "pthread.h"
#include <stdint.h>
#include <iostream>
#include <map>

using namespace std;

namespace zonda {

struct classcomp {
    bool operator()(const char *lhs, const char *rhs) const {
        return strcmp(lhs, rhs);
    }
};

class KV {
public:
    KV();
    ~KV();
    void put(char *key, char *value);
    void remove(char *key);
    void get(char *key, char *&value);

private:
    pthread_mutex_t m_mutex;
    map<char *, char *, classcomp> m_data;
};

KV::KV() {
    pthread_mutex_init(&m_mutex, NULL);
}

KV::~KV() {
    pthread_mutex_destroy(&m_mutex);
}

void KV::put(char *key, char *value) {
    pthread_mutex_lock(&m_mutex);
    m_data.insert(make_pair(key, value));
    pthread_mutex_unlock(&m_mutex);
}

void KV::remove(char *key) {
    pthread_mutex_lock(&m_mutex);
    m_data.erase(key);
    pthread_mutex_unlock(&m_mutex);
}

void KV::get(char *key, char *&value) {
    pthread_mutex_lock(&m_mutex);
    map<char *, char *, classcomp>::const_iterator iter = m_data.find(key);
    if (iter == m_data.end()) {
        value = NULL;
    } else {
        value = iter->second;
    }
    pthread_mutex_unlock(&m_mutex);
}

} //namespace zonda

void test() {
    using namespace zonda;
    KV kv;

    char *key1 = new char[24];
    sprintf(key1, ":543210");
    kv.put(key1, "rock");

    char *key2 = new char[24];
    sprintf(key2, ":543210");
    char *v = NULL;
    
    kv.get(key2, v);
    if (v == NULL) {
        cout << "::: NULL " << endl;
    } else {
        cout << "::: " << v << endl;
    }
}

int main(int argc, char *argv[]) {
    using namespace zonda;

    static const int LOOP_SIZE = 10 * 1000000; // put time: 26s/10M; get time: 11s/10M; mem: 1.2G/10M
    //static const int LOOP_SIZE = 20 * 1000000; // put time: 28s/10M; get time: 11s/10M; mem: 2.4G/20M
    //static const int LOOP_SIZE = 40 * 1000000; // put time: 29s/10M; get time: 12s/10M; mem: 4.9G/40M

    KV kv;

    time_t from = time(NULL);
    for (long i = 0; i < LOOP_SIZE; ++i) {
        char *key = new char[24];
        char *value = new char[8];
        sprintf(value, ":543210");

        sprintf(key, "ABCD>%18ld", i);
        kv.put(key, value);
        if (i / 1000000 * 1000000 == i) {
            cout << "put: " << i << endl;
        }
    }
    cout << "put time: " << (time(NULL) - from) * 10000000L / LOOP_SIZE << "s per 10M" << endl;
    cout << "mem: top..." << endl;

    from = time(NULL);
    char *key = new char[24];
    char *value = new char[8];
    for (long i = 0; i < LOOP_SIZE; ++i) {
        sprintf(key, "ABCD>%18ld", i);
        kv.get(key, value);
    }
    cout << "get time: " << (time(NULL) - from) * 10000000L / LOOP_SIZE << " per 10M" << endl;

    sleep(1000000);
    return 0;
}

