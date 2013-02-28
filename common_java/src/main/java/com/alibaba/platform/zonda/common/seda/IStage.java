package com.alibaba.platform.zonda.common.seda;



public interface IStage
{
    //! To init a stage.
    void init(Object handlerParam);

    String getName();
    IEventSink getSink();
    int getId();
    void start();

    //! stop all the threads in this stage
    void stop();
    //! Just for debug
    void enableTrace();
    void disableTrace();    
}
