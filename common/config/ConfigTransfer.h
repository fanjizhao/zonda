/**
 * @author fuping.yangfp@alibaba-inc.com
 */
#ifndef COMMON_CONFIG_CONFIGTRANSFER_H_
#define COMMON_CONFIG_CONFIGTRANSFER_H_

#include <stdint.h>

namespace zonda {
namespace common {

class HeadData {
    friend class RequestData;
    friend class ResponseData;

public:
    HeadData() : protocal_ver(-1), body_len(-1), op_type(-1), fsm_id(-1) {
    }
    enum OpType {
        REGISTER_CONFIG_LIB, REGISTER_CONFIG_LIB_RES,
        CREATE, CREATE_RES,
        DELETE, DELETE_RES,
        EXISTS, EXISTS_RES,
        GET, GET_RES,
        SET, SET_RES,
        GET_CHILDREN, GET_CHILDREN_RES,
        NOTIFY_CREATED, NOTIFY_DELETED, NOTIFY_CHANGED, NOTIFY_CHILD,
        NOTIFY_CONNECTED, NOTIFY_DISCONNECTED
    };

    int protocal_ver; // set and used by ConfigTransfer only
    int body_len; // set and used by ConfigTransfer only
    int op_type;
    int64_t fsm_id;

    int deserialize(const char *buff);
    const char *get_op_type_desc();

private:
    int hton();
    int ntoh();

private:
    static const char *op_type_desc[];
};

static const int MAX_LIB_ADDR_LEN = 64;
static const int MAX_PATH_LEN = 256;
static const int MAX_VALUE_LEN = 1024 * 10;
static const int MAX_REQUEST_LEN = sizeof(HeadData) + MAX_LIB_ADDR_LEN
                                   + MAX_PATH_LEN + sizeof(/*watch*/ bool)
                                   + sizeof(/*flags*/ int)
                                   + sizeof(/*value_len*/ int) + MAX_VALUE_LEN;

static const int MAX_RESPONSE_LEN = sizeof(HeadData) + sizeof(/*ret_code*/ int)
                                    + MAX_PATH_LEN + sizeof(/*version*/ uint64_t)
                                    + sizeof(/*cversion*/ uint64_t)
                                    + sizeof(/*strings_count*/ int)
                                    + sizeof(/*value_len*/ int) + MAX_VALUE_LEN;

class RequestData {
public:
    RequestData(): watch(false), flags(0), value_len(0) {
    }
    HeadData head;
    char lib_addr[MAX_LIB_ADDR_LEN];
    char path[MAX_PATH_LEN];
    bool watch;
    int flags;
    int value_len;
    char value[MAX_VALUE_LEN];

    int size(void);
    int serialize(char *buff);
    int deserialize(const char *buff);

private:
    int hton();
    int ntoh();
};

class ResponseData {
public:
    ResponseData() : ret_code(-1), version(0), cversion(0), strings_count(0), value_len(0) {
    }
    HeadData head;
    int ret_code;
    char path[MAX_PATH_LEN];
    uint64_t version;
    uint64_t cversion;
    int strings_count;
    int value_len;
    char value[MAX_VALUE_LEN];

    int size(void);
    int serialize(char *buff);
    int deserialize(const char *buff);

private:
    int hton();
    int ntoh();
};

} //namespace common
} //namespace zonda

#endif /* COMMON_CONFIG_CONFIGTRANSFER_H_ */

