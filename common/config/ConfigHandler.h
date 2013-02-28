/**
 * @author fuping.yangfp@alibaba-inc.com
 */
#ifndef COMMON_CONFIG_CONFIGHANDLER_H_
#define COMMON_CONFIG_CONFIGHANDLER_H_

#include <stdint.h>
#include "seda/IStage.h"
#include "seda/IEventHandler.h"
#include "util/FsmMgr.h"
#include "util/Singleton.h"
#include "thread/ThreadLock.h"
#include "thread/ThreadCond.h"
#include "ConfigChangeEvent.h"
#include "ConfigTransfer.h"
#include "ZondaConfigImpl.h"

namespace zonda {
namespace common {

//class ZondaConfigImpl;
//typedef class common::Singleton<ZondaConfigImpl> Config;

class ConfigFsm : public FsmBase<ConfigFsm> {
public:
    ConfigFsm() :
        cond(lock_for_cond), internal(false), internal_op(-1), event_sink(NULL) {
    }
    virtual ~ConfigFsm() {}
    RequestData request;
    ResponseData response;

    ThreadLock lock_for_cond;
    ThreadCond cond;

    bool internal;
    int internal_op;
    IEventSink *event_sink;
    vector<string> children;

public:
    void reset();
};

class ConfigHandler : public IEventHandler {
public:
    ConfigHandler() : m_config(NULL), m_stage(NULL) {
    }
    virtual int init(IStage *stage, void *param);
    virtual int handle_event(const IEvent *event);

private:
    ZondaConfigImpl *m_config;
    IStage *m_stage;
};

} //namespace common
} //namespace zonda
#endif /* COMMON_CONFIG_CONFIGHANDLER_H_ */

