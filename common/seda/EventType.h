#ifndef ZONDA_COMMON_EVENT_TYPE_H_
#define ZONDA_COMMON_EVENT_TYPE_H_

namespace zonda
{
namespace common
{

struct EventType
{
	//!Some predefined event types
	//!User defined event types should beging with 10001
	enum
	{
		TIMER_EVENT = 1,
		RPC_EVENT = 2,
		//Used by TransService internally
		TRANS_OUT_EVENT = 3,
		TRANS_IN_EVENT = 4,
		
		//Used by RpcService internally
		SEND_RPC_REQ_EVENT = 5,
		SEND_RPC_RES_EVENT = 6,

		CONFIG_CHANGE_EVENT = 7,
                MASTER_SLAVE_SWITCH_EVENT = 8
	};
};




}//namespace common
}//namespace zonda

#endif //ZONDA_COMMON_EVENT_H_
