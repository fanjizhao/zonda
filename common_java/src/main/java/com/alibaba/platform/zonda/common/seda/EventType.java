package com.alibaba.platform.zonda.common.seda;

public class EventType
{

        public static final int TIMER_EVENT = 1;
        public static final int RPC_EVENT = 2;
        //Used by TransService internally
        public static final int TRANS_OUT_EVENT = 3;
        public static final int TRANS_IN_EVENT = 4;

        //Used by RpcService internally
        public static final int SEND_RPC_REQ_EVENT = 5;
        public static final int SEND_RPC_RES_EVENT = 6;

        public static final int CONFIG_CHANGE_EVENT = 7;
        public static final int MASTER_SLAVE_SWITCH_EVENT = 8;
        
        
        public static final int USER_EVENT_TYPE = 1024;  
}
