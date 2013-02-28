package com.alibaba.platform.zonda.common.seda;

public interface IEventHandler
{
    void init(IStage stage, Object param);
    void handleEvent(IEvent event);
}
