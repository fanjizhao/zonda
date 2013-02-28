package com.alibaba.platform.zonda.common.rpc;

import javolution.io.Struct.Signed32;
import javolution.io.Struct;
import javolution.io.Union;


public class ServiceInstanceId extends Union {

    class InnerData extends Struct {
    	public final Unsigned32 ip = new Unsigned32();
    	public final Unsigned16 port = new Unsigned16();
    	public final Unsigned16 serviceType = new Unsigned16();
    }        
    private final InnerData  data = inner(new InnerData());
	public final Signed64 value = new Signed64();
	
	public ServiceInstanceId() {}
	public ServiceInstanceId(long id)
	{
	    value.set(id);
	}
    
    public void set(int serviceType, String ip, int port)
    {
        String[] addrArray = ip.split("\\.");
        int num = 0;
        for (int i=0;i<addrArray.length;i++) 
        {
            int power = 3-i;
            num += ((Integer.parseInt(addrArray[i])%256 * Math.pow(256,power)));
        }
        data.ip.set(num);
        data.serviceType.set(serviceType);
        data.port.set(port);
    }
    
    public void setByStr(String idStr)
    {
        
        int pos1 = idStr.indexOf('.');
        //System.out.println("idStr: " + idStr.substring(0, pos1));
        int serviceTypeNum = Integer.parseInt(idStr.substring(0, pos1));
        int pos2 = idStr.lastIndexOf('.');
        int portNum = Integer.parseInt(idStr.substring(pos2+1));
        String ipStr = idStr.substring(pos1+1, pos2);
        //System.out.println(serviceTypeNum + " " + ipStr + " " + portNum);
        set(serviceTypeNum, ipStr, portNum);
    }
    
    public String toStr()
    {
        return getServiceType() + "." + getIp() + "." + getPort();
    }
    
    public void set(long instanceId)
    {
        value.set(instanceId);
    }
    
    public String getIp()
    {
        StringBuilder ip = new StringBuilder();
        ip.append((data.ip.get() >> 24 ) & 0xFF);
        ip.append(".");
        ip.append((data.ip.get() >> 16 ) & 0xFF);
        ip.append(".");
        ip.append((data.ip.get() >> 8 ) & 0xFF);
        ip.append(".");
        ip.append( data.ip.get() & 0xFF);
        
        return ip.toString();   
    }
    
    public int getPort()
    {
        return data.port.get();
    }
    
    public int getServiceType()
    {
        return data.serviceType.get();
    }
    
    public long getValue()
    {
        return value.get();
    }
    public static void main(String[] args)
    {
        ServiceInstanceId id = new ServiceInstanceId();
        //id.set(0, "10.20.153.133", 5000);
        //id.set(432495181637948426L);
        id.setByStr("0.10.20.153.133.5000");
        String ip = id.getIp();
        int port = id.getPort();
        System.out.println("ServiceInstanceID size=" + id.size());
        System.out.println("ServiceInstanceID toString = " + id.data.toString());
        System.out.println("ServiceInstanceID value = " + id.value.get());
        System.out.println("ServiceInstanceID serviceType = " + id.getServiceType());
        System.out.println("ServiceInstanceID ip = " + ip);
        System.out.println("ServiceInstanceID port = " + port);
        

                    
    }
}
