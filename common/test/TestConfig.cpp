/**
 * @author fuping.yangfp@alibaba-inc.com
 */
#include <cstring>
#include "zookeeper.h"
#include <iostream>
#include "log/Logger.h"
#include "../config/ConfigTransfer.h"
#include "../config/Config.h"
#include "../config/Module.h"
#include "test/Assert.h"

using namespace zonda;
using namespace zonda::common;
using namespace std;


bool received_changed = false;
bool received_deleted = false;
bool received_connected = false;
bool received_disconnected = false;

class TestHandler : public IEventHandler {
public:
    virtual int init(IStage *stage, void *param);
    virtual int handle_event(const IEvent *event);
};
int TestHandler::init(IStage *stage, void *param) {
    return 0;
}
int TestHandler::handle_event(const IEvent *event) {
    int event_type = event->get_type();
    if (event_type == EventType::CONFIG_CHANGE_EVENT) {
        ConfigChangeEvent *e = (ConfigChangeEvent *)event;
        if (e->m_change_type == ConfigChangeEvent::CHANGED) {
            received_changed = true;
        } else if (e->m_change_type == ConfigChangeEvent::DELETED) {
            received_deleted = true;
        } else if (e->m_change_type == ConfigChangeEvent::CONNECTED) {
            received_connected = true;
        } else if (e->m_change_type == ConfigChangeEvent::DISCONNECTED) {
            received_disconnected = true;
        }
    }
    return 0;
}

Config *config = NULL;
Stage<TestHandler, EventQueue> stage("test", 10000);

void init(const char *service_addr) {
    config = Config::instance();
/*    if (config->init(service_addr)) {
        LOG_DEBUG("===== Wrong to init config");
        return;
    }

    if (stage.init(NULL)) {
        LOG_DEBUG("===== Wrong to init stage");
        return;
    }
    stage.start();
*/
}
/*
void basic() {
    int rtn, len;
    char buff[MAX_VALUE_LEN];
    bool existed;

    len = MAX_VALUE_LEN;
    rtn = config->get_value("/test_not_exists", buff, &len);
    assert_d("get_value test_not_exists", -101, rtn); //Node does not exist

    strcpy(buff, "test_value");
    len = sizeof("test_value");
    rtn = config->create_path("/test", buff, len, false);
    assert_d("create_path /test rtn", 0, rtn);

    strcpy(buff, "test_ephemeral");
    len = sizeof("test_ephemeral");
    rtn = config->create_path("/test/test_ephemeral", buff, len, true);
    assert_d("create_path /test/test_ephemeral rtn", 0, rtn);

    rtn = config->exists_path("/test", existed);
    assert_d("exists_path /test rtn", 0, rtn);
    assert_d("exists_path /test", true, true);

    rtn = config->exists_path("/test_not_exists", existed);
    assert_d("exists_path /test_not_exists rtn", 0, rtn);
    assert_d("exists_path /test_not_exists", false, false);

    strcpy(buff, "test_value_new");
    len = sizeof("test_value_new");
    rtn = config->set_value("/test", buff, len);
    assert_d("set_value /test rtn", 0, rtn);
    sleep(1);

    len = MAX_VALUE_LEN;
    rtn = config->get_value("/test", buff, &len);
    len = MAX_VALUE_LEN;
    rtn = config->get_value("/test", buff, &len); // Test cache
    buff[len] = '\0';
    assert_d("get_value /test rtn", 0, rtn);
    assert_s("get_value /test", "test_value_new", buff);

    vector<string> children;
    rtn = config->get_children("/test", children);
    assert_d("get_children /test rtn", 0, rtn);
    for (size_t idx = 0; idx != children.size(); ++idx) {
        assert_s("get_children /test", "test_ephemeral", children[idx].c_str());
    }

    rtn = config->delete_path("/test/test_ephemeral");
    assert_d("delete_path /test/test_ephemeral rtn", 0, rtn);

    rtn = config->delete_path("/test");
    assert_d("delete_path /test rtn", 0, rtn);
}

void watch() {
    int rtn, len;
    char buff[MAX_VALUE_LEN];
    bool existed;

    strcpy(buff, "watch_value");
    len = sizeof("watch_value");
    rtn = config->create_path("/watch", buff, len, false);
    assert_d("create_path /watch rtn", 0, rtn);

    string value;
    rtn = config->get_value("/watch", value, stage.get_sink());
    assert_d("get_value /watch rtn", 0, rtn);

    received_changed = false;
    rtn = config->set_value("/watch", "watch_value_new", strlen("watch_value_new") + 1);
    assert_d("set_value /watch rtn", 0, rtn);

    sleep(1);
    assert_d("received_changed", true, received_changed);

    rtn = config->delete_path("/watch");
    assert_d("delete_path /watch rtn", 0, rtn);
}

void test_value_change_also_trigger_exists_watch() {
    int rtn, len;
    char buff[MAX_VALUE_LEN];
    bool existed;

    strcpy(buff, "watch_value");
    len = sizeof("watch_value");
    rtn = config->create_path("/watch", buff, len, false);
    assert_d("create_path /watch rtn", 0, rtn);

    string value;
    rtn = config->exists_path("/watch", existed, stage.get_sink());
    assert_d("exists_path /watch rtn", 0, rtn);
    assert_d("exists_path /watch", true, existed);

    received_changed = false;
    rtn = config->set_value("/watch", "watch_value_new", strlen("watch_value_new") + 1);
    assert_d("set_value /watch rtn", 0, rtn);

    sleep(2);
    assert_d("received_changed", true, received_changed);

    rtn = config->delete_path("/watch");
    assert_d("delete_path /watch rtn", 0, rtn);
}

void test_delete_trigger_value_change_watch() {
    int rtn, len;
    char buff[MAX_VALUE_LEN];
    bool existed = false;

    strcpy(buff, "watch_value");
    len = sizeof("watch_value");
    rtn = config->create_path("/watch", buff, len, false);
    assert_d("create_path /watch rtn", 0, rtn);

    string value;
    rtn = config->exists_path("/watch", existed, stage.get_sink());
    assert_d("exists_path /watch rtn", 0, rtn);
    assert_d("exists_path /watch", true, existed);

    received_deleted = false;
    rtn = config->delete_path("/watch");
    assert_d("delete_path /watch rtn", 0, rtn);

    sleep(2);
    assert_d("received_deleted", true, received_deleted);
}

void test_watch_connect() {
    assert_d("received_connected", false, received_connected);
    assert_d("received_disconnected", false, received_disconnected);
    config->add_watch_connect(stage.get_sink());

    LOG_DEBUG("pls kill zk outside...");
    for (int i = 0; i < 20; ++i) {
        sleep(1);
        LOG_DEBUG("." << i + 1);
    }
    assert_d("received_connected", false, received_connected);
    assert_d("received_disconnected", true, received_disconnected);

    LOG_DEBUG("pls start zk outside...");
    for (int i = 0; i < 20; ++i) {
        sleep(1);
        LOG_DEBUG("." << i + 1);
    }
    assert_d("received_connected", true, received_connected);
}
*/
int main(int argc, char *argv[]) {
    logger::LoggerFactory::init(common::Module::CONFIG, "log.conf", false);
/*
    if (argc == 1) {
        init(NULL);
    } else {
        init(argv[1]);
    }
    basic();
    watch();
    test_value_change_also_trigger_exists_watch();
    test_delete_trigger_value_change_watch();
    test_delete_trigger_value_change_watch(); //double to check cache cleared after delete

    //test_watch_connect();

    test_done();
    */
    return 0;
}

