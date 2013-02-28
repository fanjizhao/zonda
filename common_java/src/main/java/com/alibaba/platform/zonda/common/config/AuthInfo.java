package com.alibaba.platform.zonda.common.config;

/**
 * User: darwin.yangq
 * Date: 12-1-16
 * Time: 5:34 pm
 */
public class AuthInfo {

    private String scheme;

    private byte[] auth;

    public String getScheme() {
        return scheme;
    }

    public void setScheme(String scheme) {
        this.scheme = scheme;
    }

    public byte[] getAuth() {
        return auth;
    }

    public void setAuth(byte[] auth) {
        this.auth = auth;
    }

    public String toString() {
        return (scheme == null ? "default" : scheme) + (auth == null ? 0 : auth.length);
    }
}
