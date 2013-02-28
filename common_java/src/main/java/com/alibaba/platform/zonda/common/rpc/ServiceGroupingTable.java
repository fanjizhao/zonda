package com.alibaba.platform.zonda.common.rpc;

import java.util.Map;
import java.util.HashMap;
import java.util.ArrayList;
import java.io.FileReader;
import java.io.File;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import com.alibaba.platform.zonda.logger.lib.AliLoggerFactory;

public class ServiceGroupingTable
{
    private Logger logger = AliLoggerFactory.getLogger(Constants.MODULE_NAME, getClass());
    private static ServiceGroupingTable m_instance = null;
    
    private ArrayList<HashMap<String, Integer> > m_map;
    private int m_service_count;
    
    private ServiceGroupingTable()
    {
        m_service_count = GlobalServicesInfo.getInstance().getServiceCount();
        if (m_service_count == 0)
        {
            logger.warn("No object of MsgFactory was created, use default service count 100");
            m_service_count = 100;
        }
        m_map = new ArrayList<HashMap<String, Integer> >(m_service_count);
        for (int i=0; i<m_service_count; ++i)
        {
            m_map.add(new HashMap<String, Integer>());
        }    
    }
    
    private synchronized static void createInstance()
    {
        if (m_instance == null)
        {
            ServiceGroupingTable obj = new ServiceGroupingTable();
            m_instance = obj;
        }
    }
         
    public static ServiceGroupingTable getInstance()
    {
        if (m_instance == null)
        {
            createInstance();
        }

        return m_instance;
    }    
    
    private synchronized void doRealLoad(ArrayList<HashMap<String, Integer>> arr)
    {
        for (int i=0; i<m_map.size(); ++i)
        {
            HashMap<String, Integer> hm1 = m_map.get(i);
            HashMap<String, Integer> hm2 = arr.get(i);
            hm1.clear();
            hm1.putAll(hm2);
        }
    }
    
    public static final int MAX_GROUP_COUNT = 256;
    public void load(String buff) throws Exception
    {
        logger.debug("\n" + buff);
        ArrayList<HashMap<String, Integer>> newMap = new ArrayList<HashMap<String, Integer>>(m_service_count);
        for (int i=0; i<m_service_count; ++i)
        {
            newMap.add(new HashMap<String, Integer>());
        }
        
        String[] lines = buff.split("\n");
        int serviceType = 0;
        String ip = null;
        int groupId = 0;
        for (int i=0; i<lines.length; ++i)
        {
            if (lines[i].charAt(0) == '#')
            {
                //skip the comment lines
                continue;
            }
            
            String[] fields = lines[i].split(":");
            if (fields.length != 3)
            {
                String err = "Invalid line for service grouping table,[" + lines[i] + "]";
                logger.error(err);
                throw new Exception(err);
            }
            
            serviceType = Integer.parseInt(fields[0]);
            ip = fields[1];
            groupId = Integer.parseInt(fields[2]);
            
            logger.debug("serviceType:" + serviceType
                + ", ip:" + ip + ",groupId:" + groupId);
            
            if (serviceType < 0 || serviceType >= m_service_count)
            {
                String err = "Invalid serviceType: " + serviceType 
                    + ", must be in the range of (0, " + m_service_count + ")";
                logger.error(err);
                throw new Exception(err);
            }
            
            if (groupId >= MAX_GROUP_COUNT || groupId < 0)
            {
                String err = "Invalid group_id: " + groupId 
                    + ", must be in the range of (0, 256)";
                logger.error(err);
                throw new Exception(err);
            }
            
            HashMap<String, Integer> hm = newMap.get(serviceType);
            if (hm.get(ip) != null)
            {
                String err = "Same serviceType:" + serviceType
                    + ", same ip:" + ip + ", different groupId";
                logger.error(err);
                throw new Exception(err);                
            }
            
            hm.put(ip, groupId);
        }
        
        doRealLoad(newMap);
          
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
    
    public synchronized int getGroupId(int serviceType, String ip) throws Exception
    {
        logger.debug("serviceType:" + serviceType + ", ip:" + ip);
        if (serviceType < 0 || serviceType >= m_service_count)
        {
            String err = "Invalid serviceType: " + serviceType 
                + ", must be in the range of (0, " + m_service_count + ")";
            logger.error(err);
            throw new Exception(err);            
        }
        
        HashMap<String, Integer> hm = m_map.get(serviceType);
        Integer groupId = hm.get(ip);
        if ( groupId == null)
        {
            String err = "Failed to find groupId for serviceType:"
                + serviceType + ", ip:" + ip + ", so use default value 0!";
            logger.warn(err);
            return 0;
        }
        return groupId;
    }

    public static void main(String args[])
    {
        ServiceGroupingTable gt = ServiceGroupingTable.getInstance();
        try
        {
            gt.loadFromFile("/home/jizhao/files_for_test/group_table.txt");
            System.out.println("serviceType:1, ip:192.168.0.1, groupId:" 
                + gt.getGroupId(1, "192.168.0.1"));
            System.out.println("serviceType:2, ip:10.20.153.133, groupId:" 
                + gt.getGroupId(2, "10.20.153.133"));
                
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
    }
};
