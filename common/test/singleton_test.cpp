
#include <iostream>

#include "util/Singleton.h"
#include "thread/ThreadLock.h"

class MySingletonImpl
{
public:    
    void show_id();
protected:
    MySingletonImpl();    
private:    
    int my_id;
    static int obj_count;
};

int MySingletonImpl::obj_count = 0;

MySingletonImpl::MySingletonImpl()
{
    my_id = ++obj_count;
}

void MySingletonImpl::show_id()
{
    using namespace std;
    cout << "I am " << my_id << endl;
}

typedef zonda::common::Singleton<MySingletonImpl, zonda::common::ThreadLock> MySingleton;

int main()
{
    //This statement should not be compiled.
    //MySingleton obj;
    MySingleton* p = MySingleton::instance();
    p->show_id();
}

