package com.alibaba.platform.zonda.common.util;
import com.alibaba.platform.zonda.common.seda.*;


public class TimerEvent implements IEvent
{
    private int m_id;
    private long m_param;

    public TimerEvent(int id, long param)
    {
        m_id = id;
        m_param = param;
    }
    public int getType()
    {
        return EventType.TIMER_EVENT;
    }
    public int getId()
    {
        return m_id;
    }
    public long getParam()
    {
        return m_param;
    }
};