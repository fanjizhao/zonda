package com.alibaba.platform.zonda.common.config;

/**
 * User: darwin.yangq
 * Date: 12-1-16
 * Time: 7:32 pm
 */
public class MetaInfo {

    private String connectString;
    
    private int sessionTimeout;

    public String getConnectString() {
        return connectString;
    }

    public void setConnectString(String connectString) {
        this.connectString = connectString;
    }

    public int getSessionTimeout() {
        return sessionTimeout;
    }

    public void setSessionTimeout(int sessionTimeout) {
        this.sessionTimeout = sessionTimeout;
    }
}
