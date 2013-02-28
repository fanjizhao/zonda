package com.alibaba.platform.zonda.common.test;

import com.alibaba.platform.zonda.common.seda.*;
import com.alibaba.platform.zonda.common.rpc.*;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;


class TestEvent implements IEvent
{
    public TestEvent(int p)
    {
        param = p;
    }
    public int getType()
    {
        return 2013;
    }
    
    public int param;
}



class StageTest
{
    public static void main(String argv[])
    {
        Logger logger = LoggerFactory.getLogger("StageTest");
        ServiceAddr sa = new ServiceAddr();
        System.out.println("sizeof(ServiceAddr) = " + sa.size());
        TransHead th = new TransHead();
        System.out.println("sizeof(TransHead) = " + th.size());
        
        RpcHead rh = new RpcHead();
        System.out.println("sizeof(RpcHead) = " + rh.size());
        
        //Stage s = new Stage("com.alibaba.platform.zonda.common.test.TestBizHandler", "BizStage");     
        
        Stage s = null;
        try
        {
            s = new Stage("com.alibaba.platform.zonda.common.test.TestBizHandler", 
                "BizStage", 10000, 2, StageThreadMode.EXLUSIVE);     
            for (int i=0; i<2; ++i)
            {
    
                IEvent e = new TestEvent(i);
                s.getSink().enqueue(e);
            }
            s.init(null);
            s.start();
            
            for (int i = 0; i<5; ++i)
            {
                Stage sTest = new Stage("com.alibaba.zonda.common.test.TestBizHandler", 
                    "TestStage"+i);     
                System.out.println("stage_id:" + sTest.getId() 
                    + ", stage_name:" + sTest.getName());
            }
        }
        catch (Exception e)
        {
            logger.error(e.toString());
            return;
        }
        
        try
        {
            Thread.sleep(1000*20);        
            System.out.println("Begin to stop the stage");
            s.stop();
        }
        catch (Exception e)
        {
            System.out.println(e.toString());
        }
    }
}