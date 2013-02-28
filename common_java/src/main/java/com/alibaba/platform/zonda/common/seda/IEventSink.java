package com.alibaba.platform.zonda.common.seda;

public interface IEventSink
{
    void enqueue(IEvent event);
    int size();
}
