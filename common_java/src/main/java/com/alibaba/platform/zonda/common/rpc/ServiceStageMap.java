package com.alibaba.platform.zonda.common.rpc;


import java.lang.management.ManagementFactory;
import java.lang.management.RuntimeMXBean;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import java.util.HashMap;
import java.util.HashSet;
import java.util.ArrayList;
import com.alibaba.platform.zonda.common.seda.*;

import com.alibaba.platform.zonda.common.config.Configer;
import com.alibaba.platform.zonda.common.config.CreateMode;
import com.alibaba.platform.zonda.common.config.MetaInfo;
import com.alibaba.platform.zonda.common.config.exception.ConfigUnintializedException;
import com.alibaba.platform.zonda.common.config.exception.CreateNodeFailedException;
import com.alibaba.platform.zonda.common.config.exception.SetValueFailedException;
import com.alibaba.platform.zonda.common.config.zk.ZkConfiger;
import com.alibaba.platform.zonda.logger.lib.AliLoggerFactory;

public class ServiceStageMap
{
    //private Logger //logger = AliLoggerFactory.getLogger(Constants.MODULE_NAME, getClass());
    private static ServiceStageMap m_instance = null;
    private final HashMap<Integer, IStage> m_service_stage_map = new HashMap<Integer, IStage>();
    private final HashMap<Integer, HashSet<IpAddr>> m_service_tcp_map = new HashMap<Integer, HashSet<IpAddr>>();
    private final HashMap<Integer, HashSet<String>> m_service_ipc_map = new HashMap<Integer, HashSet<String>>();
    private final HashSet<Long> m_local_instance_id_set = new HashSet<Long>();
    //The index is the serviceType, the value is the instanceId 
    private final ArrayList<Long> m_local_instance_id_list = new ArrayList<Long>();
    
    private ServiceStageMap()
    {
		for (int i=0; i<GlobalServicesInfo.getInstance().getServiceCount(); ++i)
		{
		    m_local_instance_id_list.add(null);
		}
		
    }
    private synchronized static void createInstance()
    {
        if (m_instance == null)
        {
            ServiceStageMap obj = new ServiceStageMap();
            m_instance = obj;
        }
    } 
    
    public static ServiceStageMap getInstance()
    {
        if (m_instance == null)
        {
            createInstance();
        }

        return m_instance;
    }

    public static String getLocalPid() 
    {
		// get pid
		String pid = ManagementFactory.getRuntimeMXBean().getName();
		if (pid != null && pid.length() > 1) 
		{
			pid = pid.substring(0, pid.indexOf('@'));
		} 
		else 
		{
			pid = "0000";
		}
		return pid;
	}
	
    private void sendRouteInfo(int serviceType,
        ArrayList<IpAddr> tcpAddrList,
        ArrayList<String> ipcAddrList)
    {
        String instanceName = GlobalServicesInfo.getInstance().getServiceName(serviceType)
            + "-" + tcpAddrList.get(0).ip + "-" + getLocalPid() + "-"; 
        
        //TODO: Hanlde multi ip 
        ServiceInstanceId id = new ServiceInstanceId();
        id.set(serviceType, tcpAddrList.get(0).ip, tcpAddrList.get(0).port);
        
        m_local_instance_id_list.set(serviceType, Long.valueOf(id.getValue()));
        m_local_instance_id_set.add(Long.valueOf(id.getValue()));
        
        String routeString = instanceName + "service_type=" + serviceType + "\n"
            + instanceName + "instance_id=" + id.toStr() + "\n"
            + instanceName + "tcp_addr=";
        for (int i=0; i<tcpAddrList.size(); ++i)
        {
            if (i != 0) routeString+= ",";
            routeString += tcpAddrList.get(i).ip + ":" + tcpAddrList.get(i).port;
        }
        
        routeString += "\n";
        routeString += (instanceName + "ipc_addr=\n");
        routeString += (instanceName + "state=1\n\n");


        String path = Configer.ZONDA_RUNTIME_PATH + "/" + instanceName;
		try 
		{		   
			String nodename = ZkConfiger.getInstance().create(path, 
			    routeString.getBytes(), CreateMode.EPHEMERAL_SEQUENTIAL);
			// this.configer.setValue(nodename, content.getBytes());
			//logger.debug("create node {},content:{}", nodename, routeString);

		} 
		catch (ConfigUnintializedException e) 
		{
			//logger.debug("config is not initialized,register routing failed");
			e.printStackTrace();
		} 
		catch (CreateNodeFailedException e) 
		{
			//logger.debug("create node failed, register routing failed");
			e.printStackTrace();
		}

    }

    public synchronized long getSelfInstanceId(int serviceType) throws Exception
    {
        return m_local_instance_id_list.get(serviceType);
    }
        
    public synchronized boolean isLocalService(long instanceId)
    {
        return m_local_instance_id_set.contains(Long.valueOf(instanceId));
    }
    
    public synchronized IStage getStage(int serviceType)
    {
        return m_service_stage_map.get(Integer.valueOf(serviceType));
    }
    
    public synchronized void setLocalListenAddr(ArrayList<IpAddr> tcpAddrList)
    {
        ServiceInstanceId id = new ServiceInstanceId();
        id.set(0, tcpAddrList.get(0).ip, tcpAddrList.get(0).port);
        //logger.debug("Local CLIENT instance id = " + id.getValue());
        m_local_instance_id_list.set(0, Long.valueOf(id.getValue()));
        m_local_instance_id_set.add(Long.valueOf(id.getValue()));
    }
    
    public synchronized void setStageService(IStage requestStage,
        int serviceType,
        ArrayList<IpAddr> tcpAddrList,
        ArrayList<String> ipcAddrList)
    {
        if (serviceType == 0)
        {
            //Do not need to register for CLIENT
            return;
        }
        
        if (tcpAddrList.size() == 0)
        {
            String err = "Failed to set_stage_service, tcp_addr_list is empty";
            //logger.error(err);
            throw new RuntimeException(err);
        }
        
        Integer st = new Integer(serviceType);
        if (m_service_stage_map.containsKey(st) == true)
        {
            String err = "service:" + serviceType
                + " can't be set twice";
            //logger.error(err);
            throw new RuntimeException(err);
        }
        
        m_service_stage_map.put(st, requestStage);
        HashSet<IpAddr> ipSet = new HashSet<IpAddr>();
        for (IpAddr addr: tcpAddrList)
        {
            ipSet.add(addr);
        }
        m_service_tcp_map.put(st, ipSet);
        
        //TODO: ipc addr
        
        sendRouteInfo(serviceType, tcpAddrList, ipcAddrList);
    }
    
    //Retrive all local service instance IDs without Id with serviceType == 0
    public synchronized ArrayList<Long> getLocalServiceInstanceList()
    {
        ArrayList<Long> list = new ArrayList<Long>();
        for (int i=1; i<m_local_instance_id_list.size(); ++i)
        {
            Long id = m_local_instance_id_list.get(i);
            if (id == null)
            {
                continue;
            }
            list.add(id);
        }
        
        return list;
    }
}
