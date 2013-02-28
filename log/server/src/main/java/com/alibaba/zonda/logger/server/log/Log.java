/**
 * Project: zonda.logger.server
 *
 * File Created at 12-2-2
 * Log.java
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
package com.alibaba.zonda.logger.server.log;

/**
 * User: darwin
 * Date: 12-2-2
 * Time: 3:47 pm
 */
public class Log {

    private String date;
    
    private String time;
    
    private char level;
    
    private String host;
    
    private String module;
    
    private String category;
    
    private String file;
    
    private int line = -1;
    
    private String method;
    
    private String txCode;
    
    private String threadCode;
    
    private String content;

    public Log(String date, String time, char level, String host, String module, String category, String file, int line,
               String method, String txCode, String threadCode, String content) {
        this.date = date;
        this.time = time;
        this.level = level;
        this.host = host;
        this.module = module;
        this.category = category;
        this.file = file;
        this.line = line;
        this.method = method;
        this.txCode = txCode;
        this.threadCode = threadCode;
        this.content = content;
    }

    public String toString() {
        StringBuilder sb = new StringBuilder(content.length() + 256);
        sb.append(this.date);
        sb.append(" ");
        sb.append(this.time);
        sb.append(" ");
        sb.append(this.level);
        sb.append(" [");
        sb.append(this.host);
        sb.append(":");
        sb.append(this.module);
        sb.append(":");
        sb.append(this.category);
        sb.append(":");
        sb.append(this.file);
        sb.append(":");
        sb.append(this.line);
        sb.append(":");
        sb.append(this.method);
        sb.append(":");
        sb.append(this.txCode);
        sb.append(":");
        sb.append(this.threadCode);
        sb.append(" ] - ");
        sb.append(content);
        return sb.toString();
    }

}
