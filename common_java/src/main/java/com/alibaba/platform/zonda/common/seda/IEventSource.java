package com.alibaba.platform.zonda.common.seda;

public interface IEventSource
{
    IEvent dequeue();
    IEvent timeDequeue(int millseconds);
}
