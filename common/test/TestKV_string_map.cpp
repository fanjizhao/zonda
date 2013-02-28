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

class KV {
public:
    KV();
    ~KV();
    void put(const string &key, const string &value);
    void remove(const string &key);
    void get(const string &key, string &value);

private:
    pthread_mutex_t m_mutex;
    map<string, string> m_data;
};

KV::KV() {
    pthread_mutex_init(&m_mutex, NULL);
}

KV::~KV() {
    pthread_mutex_destroy(&m_mutex);
}

void KV::put(const string &key, const string &value) {
    pthread_mutex_lock(&m_mutex);
    m_data.insert(make_pair(key, value));
    pthread_mutex_unlock(&m_mutex);
}

void KV::remove(const string &key) {
    pthread_mutex_lock(&m_mutex);
    m_data.erase(key);
    pthread_mutex_unlock(&m_mutex);
}

void KV::get(const string &key, string &value) {
    pthread_mutex_lock(&m_mutex);
    map<string, string>::const_iterator iter = m_data.find(key);
    if (iter == m_data.end()) {
        value = "";
    } else {
        value = iter->second;
    }
    pthread_mutex_unlock(&m_mutex);
}

} //namespace zonda

int main(int argc, char *argv[]) {
    using namespace zonda;

    //static const int LOOP_SIZE = 10 * 1000000; // put time: 36s/10M; get time: 21s/10M; mem: 1.7G/10M
    static const int LOOP_SIZE = 20 * 1000000; // put time: 37s/10M; get time: 21s/10M; mem: 3.4G/20M

    KV kv;
    char key_buff[20];
    char value_buff[8];
    sprintf(value_buff, ":543210");

    time_t from = time(NULL);
    for (long i = 0; i < LOOP_SIZE; ++i) {
        sprintf(key_buff, ">%18ld", i);
        kv.put(key_buff, value_buff);
        if (i / 1000000 * 1000000 == i) {
            cout << "put: " << i << endl;
        }
    }
    cout << "put time: " << (time(NULL) - from) * 10000000L / LOOP_SIZE << "s per 10M" << endl;
    cout << "mem: top..." << endl;

    string value;
    from = time(NULL);
    for (long i = 0; i < LOOP_SIZE; ++i) {
        sprintf(key_buff, ">%18ld", i);
        kv.get(key_buff, value);
    }
    cout << "get time: " << (time(NULL) - from) * 10000000L / LOOP_SIZE << " per 10M" << endl;

    sleep(1000000);
    return 0;
}

