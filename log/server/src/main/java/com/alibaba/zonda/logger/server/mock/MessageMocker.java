/**
 * Project: zonda.logger.server
 *
 * File Created at 12-2-3
 * : MessageMocker.java 12-2-3 darwin $
 *
 * Copyright 2008 Alibaba.com Croporation Limited.
 * All rights reserved.
 *
 * This software is the confidential and proprietary information of
 * Alibaba Company. ("Confidential Information").  You shall not
 * disclose such Confidential Information and shall use it only in
 * accordance with the terms of the license agreement you entered into
 * with Alibaba.com.
 */
package com.alibaba.zonda.logger.server.mock;

import com.taobao.timetunnel.client.Message;

import java.util.Set;

/**
 * User: darwin
 * Date: 12-2-3
 * Time: 5:33 pm
 */
public class MessageMocker implements Message {
    
    private String content;
    
    public MessageMocker(String content) {
        this.content = content;
    }

    @Override
    public String getTopic() {
        return null;  
    }

    @Override
    public String getId() {
        return null;  
    }

    @Override
    public byte[] getContent() {
        if (null != content) {
            return content.getBytes();
        } else {
            return new byte[0];
        }
    }

    @Override
    public long getCreatedTime() {
        return 0;  
    }

    @Override
    public String getIpAddress() {
        return null;  
    }

    @Override
    public Set<String> getAllProperties() {
        return null;  
    }

    @Override
    public String getProperty(String s) {
        return null;  
    }

    @Override
    public void compress() {
        
    }

    @Override
    public void decompress() {
        
    }

    @Override
    public boolean isCompressed() {
        return false;  
    }

    @Override
    public void deserialize(byte[] bytes) {
        
    }

    @Override
    public byte[] serialize() {
        return new byte[0];  
    }
}
