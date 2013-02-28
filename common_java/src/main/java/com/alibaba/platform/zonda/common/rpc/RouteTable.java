package com.alibaba.platform.zonda.common.rpc;

import java.net.InetSocketAddress;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.HashSet;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import java.io.FileReader;
import java.io.File;
import java.util.Iterator;
import com.alibaba.platform.zonda.common.util.NetUtil;
import com.alibaba.platform.zonda.logger.lib.AliLoggerFactory;
import com.alibaba.platform.zonda.common.seda.*;




public class RouteTable
{
    private Logger logger = AliLoggerFactory.getLogger(Constants.MODULE_NAME, getClass());
    private static RouteTable m_instance = null;

    private HashMap<Long, ServiceItem> m_service_map = new HashMap<Long, ServiceItem>();
    //Three dimensional array, service_type, group_id,
    private ArrayList<ArrayList<ArrayList<ServiceItem>>>   m_group_list;
    
    private ArrayList<ArrayList<ServiceItem>>   m_service_list;
    //the first index is the service type
    private ArrayList<ArrayList<ServiceItem>> m_local_service_list;
    //the first index is the service type
    private ArrayList<ArrayList<ServiceItem>> m_local_host_service_list;
    private final Set<String> m_local_ip_set = new HashSet<String>();
    private ArrayList<IpAddr> m_local_tcp_addr_list = new ArrayList<IpAddr>();
    private ArrayList<String> m_local_ipc_addr_list = new ArrayList<String>();
    private int m_service_count = 0;
    private int serviceRoundRobin[] = null;

    
    private synchronized static void createInstance()
    {
        if (m_instance == null)
        {
            RouteTable obj = new RouteTable();
            m_instance = obj;
        }
    }    
    
    private RouteTable()
    {
        m_service_count = GlobalServicesInfo.getInstance().getServiceCount();
        if (m_service_count == 0)
        {
            logger.warn("No object of MsgFactory was created, use default service count 100");
            m_service_count = 20;
        }
        m_group_list = new ArrayList< ArrayList<ArrayList<ServiceItem>>>(m_service_count);
        m_service_list = new ArrayList<ArrayList<ServiceItem>>(m_service_count);
        m_local_service_list = new ArrayList<ArrayList<ServiceItem>>(m_service_count);
        m_local_host_service_list = new ArrayList<ArrayList<ServiceItem>>(m_service_count);
        
        for (int i=0; i<m_service_count; ++i)
        {
            m_group_list.add(new ArrayList<ArrayList<ServiceItem>>());
            for (int j=0; j<ServiceGroupingTable.MAX_GROUP_COUNT; ++j)
            {
                m_group_list.get(i).add(new ArrayList<ServiceItem>());
            }

            m_service_list.add(new ArrayList<ServiceItem>());
            m_local_service_list.add(new ArrayList<ServiceItem>());
            m_local_host_service_list.add(new ArrayList<ServiceItem>());
        }
        
        
        NetUtil.getLocalIpAddr(m_local_ip_set);
        
        
        serviceRoundRobin = new int[m_service_count];
    }
    
    private boolean isLocalIp(String ip)
    {
        return m_local_ip_set.contains(ip);
    }
    
    private void updateGroupList()
    {
        ArrayList<Long> localInstanceList = ServiceStageMap.getInstance().getLocalServiceInstanceList();
        HashMap<Long, ServiceInstanceRole> oldRoleMap = new HashMap<Long, ServiceInstanceRole>(); 
        for (Long id: localInstanceList)
        {
            ServiceItem item = m_service_map.get(id);
            if (item == null)
            {
                logger.warn("instance_id:" + id
                    + ", is not in m_service_map");
                continue;
            }
            
            ServiceInstanceRole role = ServiceInstanceRole.UNKNOWN;
            int serviceType = item.serviceType;
            int groupId = item.groupId;
            int pos = -1;
            
            for (int j=0; j<m_group_list.get(serviceType).get(groupId).size(); ++j)
            {
                long instanceId = m_group_list.get(serviceType).get(groupId).get(j).instanceId;
                logger.debug("service_type:" + serviceType
                    + ", group_id:" + groupId
                    + ", instance_id:"
                    + instanceId);
    
                if (instanceId == id)
                {
                    pos = j;
                }
            }
            
            if (pos == 0)
            {
                role = ServiceInstanceRole.MASTER;
            }
            else if (pos > 0)
            {
                role = ServiceInstanceRole.SLAVE;
            }
            
            oldRoleMap.put(id, role);
            logger.debug("service_type:" + serviceType 
                + ", group_id:" + groupId
                + ", instance_id:" + id
                + ", old role:" + role);
        }
        
        
        
        //////////////////////////////////
        for (int i=0; i<m_service_count; ++i)
        {
            for (int j=0; j<ServiceGroupingTable.MAX_GROUP_COUNT; ++j)
            {
                m_group_list.get(i).get(j).clear();
            }
        }
        
        Iterator  entrys = m_service_map.entrySet().iterator();
        while(entrys.hasNext())
        {
            Map.Entry<Long, ServiceItem> entry = (Map.Entry<Long, ServiceItem>)entrys.next();
            ServiceItem item = entry.getValue();
            m_group_list.get(item.serviceType).get(item.groupId).add(item);
            logger.debug("serviceType: " + item.serviceType
                + ", groupId:" + item.groupId
                + ", instanceName:" + item.instanceName);            
        }

        
        //After the sort, in each group, 
        //the first will be the master, the rest will be the slaves
        for (int i=0; i<m_service_count; ++i)
        {
            for (int j=0; j<ServiceGroupingTable.MAX_GROUP_COUNT; ++j)
            {
                
                ArrayList<ServiceItem> arr = m_group_list.get(i).get(j);
                Collections.sort(arr, new CompareBySequence());
            }
        }      
          
   
        //Compare the old role and the new role for local service instances
        //To throw notification if needed
        Iterator oldRoleEntries = oldRoleMap.entrySet().iterator();
        ServiceInstanceRole oldRole;
        ServiceInstanceRole newRole;
        int routeStrategy = 0;    
    
        while(oldRoleEntries.hasNext())
        {
            Map.Entry<Long, ServiceInstanceRole> entry = (Map.Entry<Long, ServiceInstanceRole>)oldRoleEntries.next();
            oldRole = entry.getValue();
            
            ServiceItem item = m_service_map.get(entry.getKey());        
            
            int serviceType = item.serviceType;
            int groupId = item.groupId;
            
            routeStrategy = RouteStrategy.getInstance().getRouteStrategy(serviceType); 
            if (routeStrategy != RouteStrategy.MASTER_SLAVE)
            {
                continue;
            }
            
            if (m_group_list.get(serviceType).get(groupId).get(0).instanceId == entry.getKey())
            {
                newRole = ServiceInstanceRole.MASTER;
            }
            else
            {
                newRole = ServiceInstanceRole.SLAVE;
            }
    
            
            logger.debug("serviceType:" + serviceType
                + ", groupId:" + groupId
                + ", serviceCount:"
                + m_group_list.get(serviceType).get(groupId).size());
    
            logger.debug("service_type:" + serviceType 
                + ", route_strategy:" + routeStrategy
                + ", group_id:" + groupId
                + ", instance_id:" + entry.getKey()
                + ", old role:" + oldRole
                + ", new  role:" + newRole);
                
            if ( oldRole !=  newRole)
            {
                IEventSink sink = ServiceStageMap.getInstance().getStage(serviceType).getSink();
                MasterSlaveSwitchEvent event = new MasterSlaveSwitchEvent();
                event.serviceType = serviceType;
                event.oldRole = oldRole;
                event.newRole = newRole;
                sink.enqueue(event);
            }
        }    
        
    }
    
    private void updateIpcMap(ServiceItem item)
    {
    }
    
    private void updateServiceMap(ServiceItem item, HashMap<Long, ServiceItem> newMap) throws Exception
    {
        if (newMap.get(item.instanceId) != null)
        {
            String err="Duplicated service item, instance_name:" + item.instanceName
                + ", instance_id:" + item.instanceId;
            logger.error(err);
            throw new Exception(err);
        }
        
        Long id = new Long(item.instanceId);
        newMap.put(id, item); 
    }
    
    private void updateServiceList(ServiceItem item, ArrayList<ArrayList<ServiceItem>> newList)  throws Exception
    {
        newList.get(item.serviceType).add(item);
        logger.debug("service_type:" + item.serviceType
            + ", instance_id:" + item.instanceId
            + ", ip:" + item.tcpAddrList.get(0).ip
            + ", port:" + item.tcpAddrList.get(0).port);
    }
    
    private void updateLocalServiceList(ServiceItem item, ArrayList<ArrayList<ServiceItem>> newList)  throws Exception
    {
        if (! ServiceStageMap.getInstance().isLocalService(item.instanceId))
        {
            logger.debug("instance_id:" + item.instanceId 
                + " is not local service. So don't put it into local service");
            return;
        }
        newList.get(item.serviceType).add(item);
        logger.debug("instance_id:" + item.instanceId
          + " is local service, put it into the local service");        
    }
    private void updateLocalHostServiceList(ServiceItem item, ArrayList<ArrayList<ServiceItem>> newList)  throws Exception
    {
        //A service instance could not listen on IPs which are on different host
        //, so we only need to check the first IP        
        if (!isLocalIp(item.tcpAddrList.get(0).ip))
        {
            logger.debug("instance_id: " + item.instanceId
                + " is not local host service. ip:"
                + item.tcpAddrList.get(0).ip);
            return;
        }
        
        newList.get(item.serviceType).add(item);
        logger.debug("instance_id:" + item.instanceId
            + " is local host service, put it into the local host service");                    
    }
    
    private RealRpcAddr chooseRoundRobin(ServiceAddr responser) throws Exception
    {
        int serviceType = responser.serviceType.get();
        logger.debug("Service_type: " + serviceType);
        if ( serviceType >= m_service_count || serviceType <= 0)
        {
            String err="Invalid service_type:" + serviceType
                + ", m_service_count:" + m_service_count;
            logger.error(err);
            throw new Exception(err);
        }
    
        if (m_service_list.get(serviceType).size() == 0)
        {
            String err="Invalid service_type:" + serviceType
                + ", no instance";
            logger.error(err);
            throw new Exception(err);
        }
    
    
        //NOTE: Currently, we always choose the first listened tcp addr
        RealRpcAddr addr = new RealRpcAddr();
        serviceRoundRobin[serviceType] = (serviceRoundRobin[serviceType] + 1) % m_service_list.get(serviceType).size();
        int next = serviceRoundRobin[serviceType];
        addr.ip = m_service_list.get(serviceType).get(next).tcpAddrList.get(0).ip;
        addr.port = m_service_list.get(serviceType).get(next).tcpAddrList.get(0).port;
        
        logger.debug("serviceType:" + serviceType
            + ", next:" + next
            + ", " + addr.ip + ":" + addr.port);
        return addr;    
    }
    
    private RealRpcAddr chooseLocalHost(ServiceAddr responser) throws Exception
    {
        int serviceType = responser.serviceType.get();
        logger.debug("Service_type: " + serviceType);
        if ( serviceType >= m_service_count || serviceType <= 0)
        {
            String err="Invalid service_type:" + serviceType
                + ", m_service_count:" + m_service_count;
            logger.error(err);
            throw new Exception(err);
        }
    
        if (m_local_host_service_list.get(serviceType).size() == 0)
        {
            String err="Invalid service_type:" + serviceType
                + ", no instance";
            logger.error(err);
            throw new Exception(err);
        }
    
    
        //NOTE: For this strategy, we always choose the first instance's
        // first listen tcp addr
        RealRpcAddr addr = new RealRpcAddr();
        addr.ip = m_local_host_service_list.get(serviceType).get(0).tcpAddrList.get(0).ip;
        addr.port = m_local_host_service_list.get(serviceType).get(0).tcpAddrList.get(0).port;
        
        return addr;         
    }
    
    private RealRpcAddr chooseMasterSlave(ServiceAddr responser) throws Exception
    {
        int serviceType = responser.serviceType.get();
        logger.debug("Service_type: " + serviceType);
        if ( serviceType >= m_service_count || serviceType <= 0)
        {
            String err="Invalid service_type:" + serviceType
                + ", m_service_count:" + m_service_count;
            logger.error(err);
            throw new Exception(err);
        }
        
        int groupId = responser.groupId.get();
        if ( groupId >= ServiceGroupingTable.MAX_GROUP_COUNT
            || groupId < 0)
        {
            String err="Invalid groupId:" + groupId;
            logger.error(err);
            throw new Exception(err);            
        }
        
        ArrayList<ServiceItem> targetGroupList = m_group_list.get(serviceType).get(groupId);
        if (targetGroupList.size() == 0)
        {
            String err="No instance in serviceType:" + serviceType
                + ", groupId:" + groupId;
            logger.error(err);
            throw new Exception(err);
        }
        
        int pos = 0;
        if (responser.role.get() == 0)
        {
            //want master
        }
        else
        {
            //want slave
            //NOTE: Currently we only choose the first slave as the target.
            //If there is only one instance in the group, namely we choose the master
            //as the slave            
            if (targetGroupList.size() > 1) pos++;
        }

        //NOTE: This strategy is just a sample, so we always choose the first
        //tcp addr if this instance bind on multi tcp addr
        
        RealRpcAddr addr = new RealRpcAddr();
        addr.ip = targetGroupList.get(pos).tcpAddrList.get(0).ip;
        addr.port = targetGroupList.get(pos).tcpAddrList.get(0).port;
        
        return addr;
    }
    
    public static RouteTable getInstance()
    {
        if (m_instance == null)
        {
            createInstance();
        }

        return m_instance;
    }
        
    public synchronized void load(String buff) throws Exception
    {
        logger.debug("route_table len=" + buff.length() + "\n" + buff);
        if (buff.length() < 50)
        {
            logger.info("Invalid route table content, don't load it");
            return;
        }
        ServiceGroupingTable groupTable = ServiceGroupingTable.getInstance();
        HashMap<Long, ServiceItem> serviceMap = new HashMap<Long, ServiceItem>();
        ArrayList<ArrayList<ServiceItem>> serviceList, localList, localHostList; 
        serviceList = new ArrayList<ArrayList<ServiceItem>>(m_service_count);
        localList = new ArrayList<ArrayList<ServiceItem>>(m_service_count);
        localHostList = new ArrayList<ArrayList<ServiceItem>>(m_service_count);
        for (int i=0; i<m_service_count; ++i)
        {
            serviceList.add(new ArrayList<ServiceItem>());
            localList.add(new ArrayList<ServiceItem>());
            localHostList.add(new ArrayList<ServiceItem>());
        }       
        
        ServiceItem item = new ServiceItem();
        int pos;
        String[] lines = buff.split("\n");
        for (int i=0; i<lines.length; ++i)
        {
            if (lines[i].length() == 0 || lines[i].charAt(0) == '#')
            {
                //skip empty line and comment lines
                continue;
            }
            
            String[] fields = lines[i].split("=");
            if (fields.length < 1)
            {
                String err = "Invalid line, " + lines[i];
                logger.error(err);
                throw new Exception(err);
            }
            
            pos = fields[0].lastIndexOf("service_type");
            if (pos != -1)
            {
                //Find the begining of the service info
                item.instanceName = fields[0].substring(0,pos-1);
                item.serviceType = Integer.parseInt(fields[1]);
                item.updateServiceSeq();
                logger.debug("instance_name = " + item.instanceName
                    + ", service_seq=" + item.serviceSeq
                    + ", service_type:" + item.serviceType);
    
                continue;                
            }
            
            pos = fields[0].lastIndexOf("instance_id");
            if (pos != -1)
            {
                ServiceInstanceId id = new ServiceInstanceId();
                id.setByStr(fields[1]);
                item.instanceId = id.getValue();
                logger.debug("instance_id:" + fields[1]
                    + ", digital form:" + item.instanceId);
                continue;                
            }
            

            pos = fields[0].lastIndexOf("tcp_addr");
            if (pos != -1)
            {
                String[] tcp_addr_list = fields[1].split(",");
                if (tcp_addr_list.length == 0)
                {
                    String err="No tcp_addr for this service:" + item.instanceName;
                    logger.error(err);
                    throw new Exception(err);
                }
                
                for (int j=0; j<tcp_addr_list.length; ++j)
                {
                    String[] tmp = tcp_addr_list[j].split(":");
                    if (tmp.length != 2)
                    {
                        String err="Invalid tcp_addr for this service:" + item.instanceName
                            + ", tcp_addr:" + tcp_addr_list[j];
                        logger.error(err);
                        throw new Exception(err);                        
                    }
                    int port = Integer.parseInt(tmp[1]);
                    if (port < 0 || port > 65535)
                    {
                        String err="Invalid port num for this service:" + item.instanceName
                            + ", port:" + port;
                        logger.error(err);
                        throw new Exception(err);                          
                    }
                    logger.debug("tcp_addr: {}:{}", tmp[0], port);
                    IpAddr addr = new IpAddr(tmp[0], port);
                    item.tcpAddrList.add(addr);
                }
                
                //Find the group_id
                item.groupId = groupTable.getGroupId(item.serviceType, 
                    item.tcpAddrList.get(0).ip);
                logger.debug("group_id:" + item.groupId);
                continue;                
            }
            
            pos = fields[0].lastIndexOf("ipc_addr");
            if (pos != -1)
            {
                //logger.debug("ipc_addr:" + fields[1]);
                continue;                
            }            

            pos = fields[0].lastIndexOf("state");
            if (pos != -1)
            {
                item.state = Integer.parseInt(fields[1]);
                logger.debug("state:" + item.state);
                
                updateServiceMap(item, serviceMap);
                updateServiceList(item, serviceList);
                updateLocalServiceList(item, localList);
                updateLocalHostServiceList(item, localHostList);
                item = new ServiceItem();
                continue;                
            }            
            
        }
        
        //TODO: check it all the new containers have elements        
        
        
        for (int i=0; i<m_service_count; ++i)
        {
            m_service_list.get(i).clear();
            m_service_list.get(i).addAll(serviceList.get(i));
            
            
            int serviceCount = m_service_list.get(i).size();
            if (serviceCount != 0)
            {
                logger.debug("serviceType: " + i
                    + ", service instance count = "
                    + serviceCount);
            }            
            m_local_service_list.get(i).clear();
            m_local_service_list.get(i).addAll(localList.get(i));

            m_local_host_service_list.get(i).clear();
            m_local_host_service_list.get(i).addAll(localHostList.get(i));
            
        }
        
        m_service_map.clear();
        m_service_map.putAll(serviceMap);

        updateGroupList();
        
        logger.info("RouteTable has been updated successfully!");
    }
    
    public void loadFromFile(String fileName) throws Exception
    {
        File file = new File(fileName);
        FileReader reader = new FileReader(file);
        int fileLen = (int)file.length();
        char[] chars = new char[fileLen];
        reader.read(chars);
        String content = String.valueOf(chars);
        load(content);        
    }
    
    
    
    public synchronized RealRpcAddr getRealAddr(ServiceAddr responser) throws Exception
    {
        RealRpcAddr addr = null;
        
        if (responser.instanceId.get() != 0)
        {
            addr = new RealRpcAddr();
            ServiceInstanceId id = new ServiceInstanceId(responser.instanceId.get());
            addr.ip = id.getIp();
            addr.port = id.getPort();
            logger.debug("Get addr from the instance_id:" + responser.instanceId
                + ", ip:" + addr.ip + ", port:" + addr.port);
            return addr;
        }
        
        int strategy = 0;
        try 
        {
            strategy = RouteStrategy.getInstance().getRouteStrategy(responser.serviceType.get());
            
            switch(strategy)
            {
                case RouteStrategy.ROUND_ROBIN:
                    addr = chooseRoundRobin(responser);
                    break;
                case RouteStrategy.LOCALHOST:
                    addr = chooseLocalHost(responser);
                    break;
                case RouteStrategy.MASTER_SLAVE:
                    addr = chooseMasterSlave(responser);
                    break;
                default:
                    String err = "Unsupport route strategy: " 
                        + RouteStrategy.getStrategyStr(strategy);
                    throw new Exception(err);
            }
        }
        catch (Exception e)
        {
            logger.error(e.getMessage());
            throw e;
        }
        
        
        logger.debug("Got the real addr, service_type:" + responser.serviceType
            + ", instance_id:" + responser.instanceId.get()
            + ", return host:" + addr.ip
            + ", port:" + addr.port);  
        return addr;
    }
    

    
    public void setLocalListenAddr(ArrayList<IpAddr> tcpAddrVector)
    {
        m_local_tcp_addr_list.clear();
        m_local_tcp_addr_list.addAll(tcpAddrVector);        
    }
    
    public synchronized void setLocalListenAddr(ArrayList<IpAddr> tcpAddrVector, ArrayList<String> ipcAddrVector)
    {
        m_local_tcp_addr_list.clear();
        m_local_tcp_addr_list.addAll(tcpAddrVector);
        m_local_ipc_addr_list.clear();
        m_local_ipc_addr_list.addAll(ipcAddrVector);
    }
    
	public synchronized ArrayList<ServiceItem> getServiceItemList(int serviceType) 
	{
	    return m_service_list.get(serviceType);
	}
	
	public static void main(String args[])
	{
	    RouteTable rt = RouteTable.getInstance();
	    RouteStrategy rs = RouteStrategy.getInstance();
	    try
	    {
	        rs.loadFromFile("/home/jizhao/files_for_test/strategy.txt");
	        rt.loadFromFile("/home/jizhao/files_for_test/route_table.txt");
	        /*
	        ServiceAddr responser = new ServiceAddr();
	        responser.instanceId.set(730024117138948096L);
	        rt.getRealAddr(responser);
	        responser.instanceId.set(0);
	        responser.serviceType.set((short)1);
	        rt.getRealAddr(responser);
	        */
	    }
	    catch(Exception e)
	    {
	        e.printStackTrace();
	    }
	}
}


class RealRpcAddr
{
    String ip;
    int port;
    String ipcAddr;
}
