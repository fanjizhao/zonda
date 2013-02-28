package com.alibaba.platform.zonda.common.rpc;

import java.util.Vector;

public class GlobalServicesInfo
{
    private String[] m_req_name_list;
    private String[] m_res_name_list;
    private String[] m_service_name_list;
    
    //Because all msg number >= 101
    private int OFFSET = 100;
    private int m_service_count = 0; 
    private static GlobalServicesInfo m_instance = null;
    private synchronized static void createInstance()
    {
        if (m_instance == null)
        {
            GlobalServicesInfo obj = new GlobalServicesInfo();
            m_instance = obj;
        }
    }
    private GlobalServicesInfo(){}
    
    public static GlobalServicesInfo getInstance()
    {
        if (m_instance == null)
        {
            createInstance();
        }

        return m_instance;
    }
    
    public void setServiceCount(int count)
    {
        m_service_count = count;
    }
    
    public int getServiceCount()
    {
        return m_service_count;
    }
    
    public void setReqName(String[] reqNameList)
    {
        m_req_name_list = reqNameList;
    }
    
    public void setResName(String[] resNameList)
    {
        m_res_name_list = resNameList;
    }
    
    public void setServiceName(String[] serviceNameList)
    {
        m_service_name_list = serviceNameList;
    }
    
    public String getServiceName(int serviceType)
    {
        if (serviceType <0 || serviceType >= m_service_name_list.length)
        {
            return "";
        }
        
        return m_service_name_list[serviceType];
    }
    
    public String getMsgName(int msgType)
    {
        int pos = msgType > 0 ? (msgType - OFFSET) : (msgType + OFFSET);
        if (pos > 0 && pos < m_req_name_list.length)
        {
            return m_req_name_list[pos];
        }
        else if (pos < 0 && (0-pos) < m_res_name_list.length)
        {
            return m_res_name_list[0 - pos];
        }
    
        return "";        
    }  
}