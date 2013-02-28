#include "cassert"
#include <cstring>
#include "rpc/ServiceGroupingTable.h"
#include "log/Logger.h"


using namespace std;
using namespace zonda::common;

int main()
{
    zonda::logger::LoggerFactory::init("service_grouping_test", "log.conf", false);
    //const char* data = "1:192.168.1.2:1\n2:192.168.1.3:3";
    const char* data = "8:10.20.153.133:0\n8:172.16.197.87:1\n7:10.20.153.133:0\n7:172.16.197.91:1\n";
    int r = ServiceGroupingTable::instance()->load(data, strlen(data));
    assert(r == 0);
    int group_id = 0;
    /*
    group_id = ServiceGroupingTable::instance()->get_group_id(3, "192.168.1.2");
    assert(group_id == 0);
    group_id = ServiceGroupingTable::instance()->get_group_id(3, "192.168.1.4");
    assert(group_id == 0);
    group_id = ServiceGroupingTable::instance()->get_group_id(1, "192.168.1.4");
    assert(group_id == 0);
    
    group_id = ServiceGroupingTable::instance()->get_group_id(1, "192.168.1.2");
    assert(group_id == 1); 

    group_id = ServiceGroupingTable::instance()->get_group_id(1, "192.168.1.3");
    assert(group_id == 0); 

    group_id = ServiceGroupingTable::instance()->get_group_id(2, "192.168.1.3");
    assert(group_id == 3);
    */
   
    group_id = ServiceGroupingTable::instance()->get_group_id(7, "10.20.153.133");
    assert(group_id == 0);
        
}
