package com.alibaba.platform.zonda.common.util;
import com.alibaba.platform.zonda.common.seda.*;

public interface ITimerService
{    
    public int schedule(IEventSink handlerSink, long param, int delay);
    public int schedule(IEventSink handlerSink, long param, int delay, int interval);
}