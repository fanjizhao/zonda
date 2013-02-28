package com.alibaba.platform.zonda.common.config.exception;

/**
 * User: darwin.yangq
 * Date: 12-1-16
 * Time: 10:39 pm
 */
public class ConfigUnintializedException extends Exception {

    public ConfigUnintializedException(String s) {
        super(s);
    }

    public ConfigUnintializedException() {
        super();
    }

}
