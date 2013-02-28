package com.alibaba.platform.zonda.common.rpc;

import javolution.io.Struct.Signed16;
import javolution.io.Struct.Signed64;

public class ServiceAddr extends HeadBase 
{
	public final Unsigned16 serviceType = new Unsigned16(); 
	public final Unsigned16 groupId = new Unsigned16(); 
	public final Signed64 objId = new Signed64(); 
	public final Signed64 instanceId = new Signed64();  // ip+port+service_type
	public final Signed16 role = new Signed16();  //0:master 1:slave  
	public final Signed16 state = new Signed16();  //1:active 
	
	public String toPtrString() 
	{
		return serviceType + "-" + groupId + "-" +instanceId +"]";
	}

	public void set(ServiceAddr sa)
	{
        serviceType.set(sa.serviceType.get());
        groupId.set(sa.groupId.get());
        objId.set(sa.objId.get());	    	
        instanceId.set(sa.instanceId.get());
        role.set(sa.role.get());
        state.set(sa.state.get());
	}
	
}
