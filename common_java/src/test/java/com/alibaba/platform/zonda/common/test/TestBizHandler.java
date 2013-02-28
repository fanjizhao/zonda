package com.alibaba.platform.zonda.common.test;

import com.alibaba.platform.zonda.common.seda.*;

public class TestBizHandler implements IEventHandler
{
    private int handleNum = 0;
    private IStage myStage = null;
    private synchronized void incHanldeNum()
    {
        handleNum++;
    }
    public void init(IStage stage, Object param)
    {
        myStage = stage;
        System.out.println("BizHandler.init() ....");

    }
    
    public void handleEvent(IEvent event)
    {
        incHanldeNum();
        TestEvent e = (TestEvent)event;
        System.out.println("Thread:" + Thread.currentThread().getName() +
            ", Handing Event Type: " + e.getType()
            + ", param:" + e.param
            + ", handleNum:" + handleNum);
    }
}