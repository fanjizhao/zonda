
#include <cstdlib>
#include "util/MsgPool.h"
#include "rpc/GlobalServicesInfo.h"
#include "MsgFactory.h"
#include "services.h"


namespace zonda
{

MsgFactory::MsgFactory()
{
    zonda::common::GlobalServicesInfo::instance()->set_service_count(SERVICE_COUNT);
    zonda::common::GlobalServicesInfo::instance()->set_req_name(req_name_list);
    zonda::common::GlobalServicesInfo::instance()->set_res_name(res_name_list);
    zonda::common::GlobalServicesInfo::instance()->set_service_name(service_name_list);
    
    
}
    
int MsgFactory::serialize_body(uint8_t* buff, int buff_len, 
    zonda::common::IMsg *msg, int& data_len, bool keep_msg)
{
    if(buff == NULL || msg == NULL)
    {
        return -1;
    }
    
    int type = msg->get_msg_type();
    int r = 0;
    switch(type)
    {
        #define _MSG_TYPE_DEAL_FUN(Type, Type_val) \
            case Type_val:\
            {\
                    r = msg->serialize(buff, buff_len, data_len);\
                    if (r==0 && (!keep_msg) ) msg->free();\
            }\
            break ;\

         #include "./class_msg_type-inc.h"


        #undef _MSG_TYPE_DEAL_FUN

        default:
            return -1; 
    }
    
    return r;
    
}

zonda::common::IMsg* MsgFactory::deserialize_body(uint8_t* buff, int buff_len, int msg_type)
{
    if( buff == NULL)
    {
        return NULL;
    }
    
    switch (msg_type)
    {
        #define _MSG_TYPE_DEAL_FUN(Type, Type_val) \
            case Type_val:\
            {\
                    zonda::common::IMsg* msg = Type::create();\
                    int r = msg->deserialize(buff, buff_len);\
                    if (r != 0)\
                    {\
                        msg->free();\
                        return NULL;\
                    }\
                    else\
                    {\
                        return msg;\
                    }\
            }\
            break ;\
            
         #include "./class_msg_type-inc.h"


        #undef _MSG_TYPE_DEAL_FUN

        default:
            return NULL;       
    }
}

}//namespace zonda


