package com.alibaba.platform.zonda.common.rpc;

import java.util.Comparator;
import java.net.InetSocketAddress;
import java.util.ArrayList;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import com.alibaba.platform.zonda.logger.lib.AliLoggerFactory;


public class ServiceItem 
{
    private static Logger logger = AliLoggerFactory.getLogger(Constants.MODULE_NAME, ServiceItem.class);
    
	public String instanceName; 
	public int serviceType = 0;
	public int groupId = 0;
	public long instanceId; 
	public final ArrayList<IpAddr> tcpAddrList = new ArrayList<IpAddr>(); 
	public final ArrayList<String> ipcAddrList = new ArrayList<String>();
	public int state = 1;
	
	public long serviceSeq = 0;
	public void updateServiceSeq()
	{
	    int pos;
	    pos = instanceName.lastIndexOf('-');
	    if (pos == -1)
	    {
	        logger.error("Failed to find the sequence for " 
	            + instanceName);
            return;
	    }
	    String seqStr = instanceName.substring(pos+1);
	    serviceSeq = Long.parseLong(seqStr);
	}
	public void reset()
	{
	    tcpAddrList.clear();
	    ipcAddrList.clear();
	}
}

class CompareBySequence implements Comparator<ServiceItem>
{
    public int compare(ServiceItem item1, ServiceItem item2)
    {
      
        if (item1.serviceSeq < item2.serviceSeq)
        {
            return -1;
        }
        
        return 1;
    }
}



