/**
 * @author fuping.yangfp@alibaba-inc.com
 */
#include <iostream>
#include "log/Logger.h"
#include "../config/ConfigTransfer.h"
#include "../config/Config.h"
#include "../config/Module.h"
#include "Assert.h"

using namespace std;
using namespace zonda;
using namespace zonda::common;

#define ps(s1, s2, s3) assert_s(#s3, s1.s3, s2.s3)
#define pd(s1, s2, s3) assert_d(#s3, s1.s3, s2.s3)

int main() {
    logger::LoggerFactory::init(common::Module::CONFIG, "log.conf", false);

    // test request
    RequestData request;
    request.head.op_type = 300;
    request.head.fsm_id = 46912520615088L;
    strcpy(request.lib_addr, "ro ck");
    strcpy(request.path, "ro ck2");
    request.watch = true;
    request.flags = Config::NODE_EPHEMERAL;
    request.value_len = 6;
    strcpy(request.path, "ro ck3");
    
    char buff[MAX_REQUEST_LEN];
    request.serialize(buff);

    RequestData request2;
    request2.deserialize(buff);
    pd(request, request2, head.protocal_ver);
    pd(request, request2, head.body_len);
    pd(request, request2, head.op_type);
    pd(request, request2, head.fsm_id);
    ps(request, request2, lib_addr);
    ps(request, request2, path);
    pd(request, request2, watch);
    pd(request, request2, flags);
    pd(request, request2, value_len);
    ps(request, request2, value);

    //test response
    ResponseData response;
    response.head.op_type = 300;
    response.head.fsm_id = 46912520615088L;
    response.ret_code = 302;
    strcpy(response.path, "ro ck");
    response.version = 303;
    response.cversion = 304;
    response.strings_count = 305;
    response.value_len = 6;
    strcpy(response.value, "ro ck2");

    response.serialize(buff);

    ResponseData response2;
    response2.deserialize(buff);
    pd(response, response2, head.protocal_ver);
    pd(response, response2, head.body_len);
    pd(response, response2, head.op_type);
    pd(response, response2, head.fsm_id);
    pd(response, response2, ret_code);
    ps(response, response2, path);
    pd(response, response2, version);
    pd(response, response2, cversion);
    pd(response, response2, strings_count);
    pd(response, response2, value_len);
    ps(response, response2, value);

    test_done();

    return 0;
}

