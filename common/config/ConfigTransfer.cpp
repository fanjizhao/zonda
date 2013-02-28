/**
 * @author fuping.yangfp@alibaba-inc.com
 */
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include "log/Logger.h"
#include "util/byte_order.h"
#include "ConfigTransfer.h"

using namespace zonda::common;

const char *HeadData::op_type_desc[] = {
    "REGISTER_CONFIG_LIB", "REGISTER_CONFIG_LIB_RES",
    "CREATE", "CREATE_RES",
    "DELETE", "DELETE_RES",
    "EXISTS", "EXISTS_RES",
    "GET", "GET_RES",
    "SET", "SET_RES",
    "GET_CHILDREN", "GET_CHILDREN_RES",
    "NOTIFY_CREATED", "NOTIFY_DELETED", "NOTIFY_CHANGED", "NOTIFY_CHILD",
    "NOTIFY_CONNECTED", "NOTIFY_DISCONNECTED"
};

int HeadData::hton() {
    protocal_ver = htonl(protocal_ver);
    body_len = htonl(body_len);
    op_type = htonl(op_type);
    fsm_id = my_htonll(fsm_id);
    return 0;
}

int HeadData::ntoh() {
    protocal_ver = ntohl(protocal_ver);
    body_len = ntohl(body_len);
    op_type = ntohl(op_type);
    fsm_id = my_ntohll(fsm_id);
    return 0;
}

int HeadData::deserialize(const char *buff) {
    memcpy(this, buff, sizeof(*this));
    ntoh();
    return 0;
}

const char *HeadData::get_op_type_desc() {
    return op_type_desc[op_type];
}

int RequestData::hton() {
    head.hton();
    flags = htonl(flags);
    value_len = htonl(value_len);
    return 0;
}

int RequestData::ntoh() {
    head.ntoh();
    flags = ntohl(flags);
    value_len = ntohl(value_len);
    return 0;
}

int ResponseData::hton() {
    head.hton();
    ret_code = htonl(ret_code);
    version = htonl(version);
    cversion = htonl(cversion);
    strings_count = htonl(strings_count);
    value_len = htonl(value_len);
    return 0;
}

int ResponseData::ntoh() {
    head.ntoh();
    ret_code = ntohl(ret_code);
    version = ntohl(version);
    cversion = ntohl(cversion);
    strings_count = ntohl(strings_count);
    value_len = ntohl(value_len);
    return 0;
}

int RequestData::size() {
    return sizeof(head) + strlen(lib_addr) + 1 + strlen(path) + 1 + sizeof(watch)
           + sizeof(flags) + sizeof(value_len) + value_len;
}

int RequestData::serialize(char *buff) {
    head.protocal_ver = 1;
    head.body_len = size();
    int value_lenth = value_len;

    hton();
    int from = 0;

    memcpy(buff + from, &head, sizeof(head));
    from += sizeof(head);

    strcpy(buff + from, lib_addr);
    from += strlen(lib_addr) + 1;

    strcpy(buff + from, path);
    from += strlen(path) + 1;

    memcpy(buff + from, &watch, sizeof(watch));
    from += sizeof(watch);

    memcpy(buff + from, &flags, sizeof(flags));
    from += sizeof(flags);

    memcpy(buff + from, &value_len, sizeof(value_len));
    from += sizeof(value_len);

    memcpy(buff + from, &value, value_lenth);
    from += value_lenth;

    ntoh();
    return 0;
}

int RequestData::deserialize(const char *buff) {
    int from = 0;

    memcpy(&head, buff + from, sizeof(head));
    from += sizeof(head);

    strcpy(lib_addr, buff + from);
    from += strlen(lib_addr) + 1;

    strcpy(path, buff + from);
    from += strlen(path) + 1;

    memcpy(&watch, buff + from, sizeof(watch));
    from += sizeof(watch);

    memcpy(&flags, buff + from, sizeof(flags));
    from += sizeof(flags);

    memcpy(&value_len, buff + from, sizeof(value_len));
    from += sizeof(value_len);

    int value_lenth = ntohl(value_len);
    memcpy(&value, buff + from, value_lenth);
    from += value_lenth;

    ntoh();
    if (from != head.body_len) {
        LOG_ERROR("Wrong request data. body_len="
                  << head.body_len << "; received=" << from);
        return -1;
    }
    return 0;
}

int ResponseData::size() {
    return sizeof(head) + sizeof(ret_code) + strlen(path) + 1 + sizeof(version)
           + sizeof(cversion) + sizeof(strings_count) + sizeof(value_len) + value_len;
}

int ResponseData::serialize(char *buff) {
    head.protocal_ver = 1;
    head.body_len = size();
    int value_lenth = value_len;

    hton();
    int from = 0;

    memcpy(buff + from, &head, sizeof(head));
    from += sizeof(head);

    memcpy(buff + from, &ret_code, sizeof(ret_code));
    from += sizeof(ret_code);

    strcpy(buff + from, path);
    from += strlen(path) + 1;

    memcpy(buff + from, &version, sizeof(version));
    from += sizeof(version);

    memcpy(buff + from, &cversion, sizeof(cversion));
    from += sizeof(cversion);

    memcpy(buff + from, &strings_count, sizeof(strings_count));
    from += sizeof(strings_count);

    memcpy(buff + from, &value_len, sizeof(value_len));
    from += sizeof(value_len);

    memcpy(buff + from, &value, value_lenth);
    from += value_lenth;

    ntoh();
    return 0;
}

int ResponseData::deserialize(const char *buff) {
    int from = 0;

    memcpy(&head, buff + from, sizeof(head));
    from += sizeof(head);

    memcpy(&ret_code, buff + from, sizeof(ret_code));
    from += sizeof(ret_code);

    strcpy(path, buff + from);
    from += strlen(path) + 1;

    memcpy(&version, buff + from, sizeof(version));
    from += sizeof(version);

    memcpy(&cversion, buff + from, sizeof(cversion));
    from += sizeof(cversion);

    memcpy(&strings_count, buff + from, sizeof(strings_count));
    from += sizeof(strings_count);

    memcpy(&value_len, buff + from, sizeof(value_len));
    from += sizeof(value_len);

    int value_lenth = ntohl(value_len);
    memcpy(&value, buff + from, value_lenth);
    from += value_lenth;

    ntoh();
    if (from != head.body_len) {
        LOG_ERROR("Wrong response data. body_len="
                  << head.body_len << "; received=" << from);
        return -1;
    }
    return 0;
}


