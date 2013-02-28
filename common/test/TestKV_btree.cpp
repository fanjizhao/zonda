/**
 * @author fuping.yangfp@alibaba-inc.com
 */
#include "sys/time.h"
#include "pthread.h"
#include <stdint.h>
#include <iostream>
#include <fstream>
#include "stx/btree_map.h"

using namespace std;

namespace zonda {

class Key {
public:
    Key(uint64_t k1, uint64_t k2) : int1(k1), int2(k2) {}
    Key(const Key &key1) : int1(key1.int1), int2(key1.int2) {}
    Key() {}

    uint64_t int1;
    uint64_t int2;
};

struct ClassComp {
    bool operator()(const Key &key1, const Key &key2) const {
        return key1.int1 < key2.int1 || key1.int1 == key2.int1 && key1.int2 < key2.int2;
    }
};

class KV {
public:
    KV();
    ~KV();
    void put(const Key &key, uint32_t value);
    void remove(const Key &key);
    void get(const Key &key, uint32_t &value);
    void dump(std::ostream &os);
    void restore(std::istream &is);

private:
    pthread_mutex_t m_mutex;
    stx::btree_map<Key, uint32_t, ClassComp> m_data;
};

KV::KV() {
    pthread_mutex_init(&m_mutex, NULL);
}

KV::~KV() {
    pthread_mutex_destroy(&m_mutex);
}

void KV::put(const Key &key, uint32_t value) {
    pthread_mutex_lock(&m_mutex);
    m_data.insert(make_pair(key, value));
    pthread_mutex_unlock(&m_mutex);
}

void KV::remove(const Key &key) {
    pthread_mutex_lock(&m_mutex);
    m_data.erase(key);
    pthread_mutex_unlock(&m_mutex);
}

void KV::get(const Key &key, uint32_t &value) {
    pthread_mutex_lock(&m_mutex);
    stx::btree_map<Key, uint32_t, ClassComp>::const_iterator iter = m_data.find(key);
    if (iter == m_data.end()) {
        value = 0;
    } else {
        value = iter->second;
    }
    pthread_mutex_unlock(&m_mutex);
}

void KV::dump(std::ostream &os) {
    pthread_mutex_lock(&m_mutex);
    m_data.dump(os);
    pthread_mutex_unlock(&m_mutex);
}

void KV::restore(std::istream &is) {
    pthread_mutex_lock(&m_mutex);
    //    m_data.restore(is);
    pthread_mutex_unlock(&m_mutex);
}
} //namespace zonda

int main(int argc, char *argv[]) {
    using namespace zonda;

    static const int LOOP_SIZE = 10 * 1000000;
    KV kv;
    /*
    uint32_t v;
    kv.put(Key(1, 2), 9);
    kv.get(Key(1, 2), v);
    cout << v << endl;
    return 0;
    */
    time_t from = time(NULL);
    Key key(1, 1);
    for (uint32_t i = 0; i < LOOP_SIZE; ++i) {
        key.int1 += i;
        key.int2 += i;
        kv.put(key, i);
        if (i / 1000000 * 1000000 == i) {
            cout << "put: " << i << endl;
        }
    }
    cout << "put time: " << (time(NULL) - from) * 10000000L / LOOP_SIZE << "s per 10M" << endl;
    cout << "mem: top..." << endl;

    Key k1(2, 2);
    uint32_t v1;
    kv.get(k1, v1);
    cout << "k1(1): " << v1 << endl;

    std::fstream fout("/tmp/rock.b");
    kv.dump(fout);
    fout.close();

    //  std::fstream fin("/tmp/rock.b");
    //  kv.restore(fin);
    //  fout.close();

    from = time(NULL);
    Key key2(1, 1);
    uint32_t value;
    for (uint32_t i = 0; i < LOOP_SIZE; ++i) {
        key2.int1 += i;
        key2.int2 += i;
        kv.get(key2, value);
    }
    cout << "get time: " << (time(NULL) - from) * 10000000L / LOOP_SIZE << " per 10M" << endl;

    sleep(1000000);
    return 0;
}

