/**
 * @author fuping.yangfp@alibaba-inc.com
 */
#include <strings.h>
#include "seda/EventType.h"
#include "util/TransInEvent.h"
#include "ZondaConfigImpl.h"
#include "ConfigHandler.h"
#include "log/Logger.h"
#include "Config.h"

using namespace zonda::common;
using namespace std;

void ConfigFsm::reset() {
    bzero(&request, sizeof(request));
    bzero(&response, sizeof(response));
    internal = false;
    event_sink = NULL;
    children.clear();
}

int ConfigHandler::init(IStage *stage, void *param) {
    m_stage = stage;
    m_config = (Config *) param;
    return 0;
}

int ConfigHandler::handle_event(const IEvent *event) {
    int event_type = event->get_type();
    if (event_type == EventType::TRANS_IN_EVENT) {
        TransInEvent *e = (TransInEvent *)event;
        HeadData head;
        head.deserialize((const char *) e->get_buff());
        LOG_DEBUG("Received event type TRANS_IN_EVENT with op_type "
                  << head.get_op_type_desc()
                  << "(" << head.op_type << ")");

        if (head.op_type == HeadData::REGISTER_CONFIG_LIB_RES
            || head.op_type == HeadData::CREATE_RES
            || head.op_type == HeadData::DELETE_RES
            || head.op_type == HeadData::EXISTS_RES
            || head.op_type == HeadData::GET_RES
            || head.op_type == HeadData::SET_RES
            || head.op_type == HeadData::GET_CHILDREN_RES) {
            m_config->handle_res((const char *) e->get_buff(), e->get_buff_len(), head.fsm_id);
        } else if (head.op_type == HeadData::NOTIFY_CONNECTED
                   || head.op_type == HeadData::NOTIFY_DISCONNECTED
                   || head.op_type == HeadData::NOTIFY_CREATED
                   || head.op_type == HeadData::NOTIFY_DELETED
                   || head.op_type == HeadData::NOTIFY_CHANGED
                   || head.op_type == HeadData::NOTIFY_CHILD) {
            m_config->notify_data((const char *) e->get_buff(), e->get_buff_len());
        } else {
            LOG_ERROR("Get wrong op_type " << head.op_type);
        }
    } else {
        LOG_ERROR("Received wrong event type " << event_type);
    }
    delete event;
    return 0;
}


