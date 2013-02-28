package com.alibaba.platform.zonda.common.rpc;

import com.alibaba.platform.zonda.common.seda.*;


public class MasterSlaveSwitchEvent implements IEvent
{
    public int getType()
    {
        return EventType.MASTER_SLAVE_SWITCH_EVENT;
    }
    
    public int serviceType;
    public ServiceInstanceRole oldRole;
    public ServiceInstanceRole newRole;
}