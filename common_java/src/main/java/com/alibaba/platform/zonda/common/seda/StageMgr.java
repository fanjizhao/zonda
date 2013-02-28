package com.alibaba.platform.zonda.common.seda;

import java.util.Vector;
import java.util.HashMap;
import java.util.Map;

public class StageMgr
{
    private static StageMgr m_instance = null;
    private Vector<IStage>  m_stage_list = new Vector<IStage>();
    private Map<String, IStage> m_stage_map = new HashMap<String, IStage>();   
    
    private synchronized static void createInstance()
    {
        if (m_instance == null)
        {
            StageMgr obj = new StageMgr();
            m_instance = obj;
        }
    }
    private StageMgr() {}
    
 
    public static StageMgr getInstance()
    {
        if (m_instance == null)
        {
            createInstance();
        }
        
        return m_instance;
    }
    
    
      
    public synchronized IStage getStage(String stageName)
    {
        return m_stage_map.get(stageName);
    }
    
    public synchronized IStage getStage(int stageId)
    {
        try
        {
            return m_stage_list.get(stageId);
        }
        catch (ArrayIndexOutOfBoundsException  e)
        {
            return null;
        }
    }
    
    public synchronized int regStage(IStage stage)
    {
        if (m_stage_map.get(stage.getName()) != null)
        {
            throw new RuntimeException("stage:" + stage.getName() + " duplicated!");
        }
        
        m_stage_map.put(stage.getName(), stage);
        int id = m_stage_list.size();
        m_stage_list.add(stage);
        
        return id;
    }

    public synchronized void deregStage(IStage stage)
    {
        m_stage_map.remove(stage);
        m_stage_list.set(stage.getId(), null);
    }
    
    public synchronized void stop_all()
    {
        for (int i=0; i<m_stage_list.size(); ++i)
        {
            IStage s = m_stage_list.get(i);
            if (s != null) s.stop();
        }    
    }
}
