#include <getopt.h>
#include <cstring>
#include "log/Logger.h"
#include "config/Config.h"
#include "config.h"
#include "thread/ThreadMgr.h"
#include "seda/Stage.h"
#include "BizHandler.h"


using namespace std;
using namespace zonda;
using namespace zonda::common;

char conf_service_addr[1024] = "";

int main(int argc, char** argv)
{
    int r = 0;
    zonda::logger::LoggerFactory::init("route_admin", "route_admin_log.conf", false);
    LOG_DEBUG("route_admin starting ...");
    int c;
    int digit_optind = 0;
    
    while (1) 
    {
       int option_index = 0;
       static struct option long_options[] = {
           {"version", 0, 0, 0},
           {"conf_addr", 1, 0, 0},
           {"warm", 1, 0, 0},
           {0, 0, 0, 0}
       };
    
       c = getopt_long (argc, argv, "vc:w:",
                long_options, &option_index);
       if (c == -1)
           break;
    
       switch (c) 
       {
       case 'v':
           printf ("route_admin version %s\n", PACKAGE_VERSION);
           break;
    
       case 'c':
           printf ("option c with value '%s'\n", optarg);
           strcpy(conf_service_addr, optarg);
           break;
    
       case 'w':
           printf ("option w with value '%s'\n", optarg);
           break;
    
       default:
           printf ("ambiguous option\n");
           break;
       }
    }


    
    
    Config *config = Config::instance();
    r = config->init(conf_service_addr);
    if (r != 0)
    {
        LOG_FATAL("Failed to init config lib, process exit!");
        return -1;
    }
    
    zonda::common::Stage<BizHandler> biz_stage("route_admin_biz_stage", 10000);
    r = biz_stage.init(NULL);
    if (r != 0)
    {
        LOG_FATAL("Failed to init biz_stage");
        return -1;
    }
    
    r = biz_stage.start();
    if (r != 0)
    {
        LOG_FATAL("Failed to start biz_stage");
        return -1;        
    }
    
    zonda::common::ThreadMgr::instance()->wait_all();
    
    return 0;
}
