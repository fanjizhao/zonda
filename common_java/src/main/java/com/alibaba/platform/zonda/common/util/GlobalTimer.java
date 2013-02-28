package com.alibaba.platform.zonda.common.util;

import com.alibaba.platform.zonda.common.seda.*;
import java.util.Timer;
import java.util.TimerTask;
import java.util.Date;




public class GlobalTimer implements ITimerService
{
    private static GlobalTimer m_instance = null;
    private Timer m_timer = new Timer("GlobalTimer");
    private int m_id_seq = 0;
    
    private GlobalTimer()
    {
    }
    
    public static GlobalTimer getInstance()
    {
        if (m_instance == null)
        {
            synchronized(GlobalTimer.class)
            {
                if (m_instance == null)
                {
                    GlobalTimer obj = new GlobalTimer();
                    m_instance = obj;
                }
            }
        }
        
        return m_instance;
    }
    
    //Time unit is millsecond
    public int schedule(IEventSink handlerSink, long param, int delay)
    {
        return schedule(handlerSink, param, delay, 0);
    }
    
    //Time unit is millsecond
    public synchronized int schedule(IEventSink handlerSink, long param, int delay, int interval)
    {
        TimerItem item = new TimerItem();
        item.id = ++m_id_seq;
        item.param = param;
        item.interval = interval;
        item.sink = handlerSink;
        item.expireTime = new Date(System.currentTimeMillis() + delay);
        if (interval != 0)
        {
            m_timer.schedule(item, delay, interval);
        }
        else
        {
            m_timer.schedule(item, delay);
        }
        
        return item.id;
    }
    
    public static void main(String[] args)
    {
        GlobalTimer.getInstance().schedule(null, 888, 1000, 1000);
    }  
}


class TimerItem extends TimerTask
{
    public int id;
    public int interval;
    public long param;
    public Date expireTime;
    public IEventSink sink;
    public void run()
    {
        //System.out.println("timer id:" + id
        //    + ", param:" + param
        //    + ", expireTime:" + expireTime.getTime()
        //    + ", now:" + System.currentTimeMillis());
        TimerEvent e = new TimerEvent(id, param);
        sink.enqueue(e);
    }
}