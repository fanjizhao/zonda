package com.alibaba.platform.zonda.common.seda;

public enum StageThreadMode
{
    //!All threads shared one event handler object. 
    //!That means the handler must be reentrant. 
    //!In other words, it must be thread-safe. 
    SHARED,

    //! Each thread holds one event handler object.
    EXLUSIVE
}
