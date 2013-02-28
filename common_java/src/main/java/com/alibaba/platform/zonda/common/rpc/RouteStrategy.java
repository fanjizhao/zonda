package com.alibaba.platform.zonda.common.rpc;


import java.util.Map;
import java.util.HashMap;
import java.util.ArrayList;
import java.io.FileReader;
import java.io.File;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import com.alibaba.platform.zonda.logger.lib.AliLoggerFactory;


public class RouteStrategy
{
    private Logger logger = AliLoggerFactory.getLogger(Constants.MODULE_NAME, getClass());
    private static RouteStrategy m_instance = null;
    private int m_service_count;
    private final ArrayList<Integer> m_array;
    
    private RouteStrategy()
    {
        m_service_count = GlobalServicesInfo.getInstance().getServiceCount();
        if (m_service_count == 0)
        {
            logger.warn("No object of MsgFactory was created, use default service count 100");
            m_service_count = 100;
        }
        m_array = new ArrayList<Integer>(m_service_count);
        
        for (int i=0; i<m_service_count; ++i)
        {
            m_array.add(null);
        }
        
    }
    
    private synchronized static void createInstance()
    {
        if (m_instance == null)
        {
            RouteStrategy obj = new RouteStrategy();
            m_instance = obj;
        }
    }
    
    private static String m_strategy_str[] = { "UNSUPPORTED", 
            "DIRECT", "MODULO", "Consistent Hash", "Round Robin",
        "LOCALHOST", "MASTER_SLAVE"}; 
    
    private synchronized void doRealLoad(ArrayList<Integer> newArray)
    {
        for (int i=0; i<newArray.size(); ++i)
        {
            m_array.set(i, newArray.get(i));
        }
    }
         
    public static RouteStrategy getInstance()
    {
        if (m_instance == null)
        {
            createInstance();
        }

        return m_instance;
    }

    public static final int UNSUPPORTED = 0;
    public static final int DIRECT = 1;
    public static final int MODULO = 2;
    public static final int CHASH = 3;
    public static final int ROUND_ROBIN = 4;
    public static final int LOCALHOST = 5;
    public static final int MASTER_SLAVE = 6;
    public static final int MAX_STRATEGY = 7;
    
    public void load(String buff) throws Exception
    {
        logger.debug("\n" + buff);        
        ArrayList<Integer> newArray = new ArrayList<Integer>(m_service_count);
        for (int i=0; i<m_service_count; ++i)
        {
            newArray.add(null);
        }
        String[] lines = buff.split("\n");
        int serviceType = 0;
        int strategy = 0;
        
        for (int i=0; i<lines.length; ++i)
        {
            if (lines[i].charAt(0) == '#')
            {
                //skip comment lines
                continue;
            }
            
            String fields[] = lines[i].split("=");
            if (fields.length != 2)
            {
                String err = "Invalid line, [" + lines[i] + "]";
                logger.error(err);
                throw new Exception(err);
            }
            
            serviceType = Integer.parseInt(fields[0]);
            strategy = Integer.parseInt(fields[1]);
            
            if (serviceType < 0 || serviceType >= m_service_count)
            {
                String err = "Invalid serviceType," + serviceType;
                logger.error(err);
                throw new Exception(err);
            }

            if (strategy < 0 || strategy >= MAX_STRATEGY)
            {
                String err = "Invalid strategy," + strategy;
                logger.error(err);
                throw new Exception(err);
            }
            
            newArray.set(serviceType, new Integer(strategy));            
        }
        
        if (newArray.size() <= 0)
        {
            logger.warn("No valid lines found, give up the update");
            return;
        }
        
        doRealLoad(newArray);
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
        
    public synchronized int getRouteStrategy(int serviceType)
    {
        if (serviceType == 0) //For client
        {
            return DIRECT;
        }
        
        if (serviceType < 0 || serviceType > m_service_count)
        {
            return UNSUPPORTED;
        }
        
        Integer i = m_array.get(serviceType);
        if (i == null) return UNSUPPORTED;
        return i.intValue();
    }
    
    public static String getStrategyStr(int strategy)
    {
        if (strategy < 0 || strategy >= MAX_STRATEGY)
        {
            return "UNKNOW_STRATEGY";
        }
        
        return m_strategy_str[strategy];
    }
    
    
    public static void main(String args[])
    {
        RouteStrategy rs = RouteStrategy.getInstance();
        try
        {
            rs.loadFromFile("/home/jizhao/files_for_test/strategy.txt");
            for (int i=0; i<MAX_STRATEGY; ++i)
            {
                System.out.println("strategy:" + i + ", " + rs.getStrategyStr(i));
            }
            
            for (int i=0; i<rs.m_service_count; ++i)
            {
                System.out.println("serviceType:" +i +", strategy:" + rs.getRouteStrategy(i));
            }
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
    }
       
}
