/**
 * Project: zonda.logger.server
 *
 * File Created at 2011-12-17
 * $Id: Configure.java 2011-12-17 $
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
package com.alibaba.zonda.logger.server.config;

import java.io.InputStream;
import java.util.Properties;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * User: darwin
 * Date: 11-12-17
 * Time: 3:47 pm
 */
public class Configure {

    private static Logger logger = LoggerFactory.getLogger(Configure.class);
    private static Configure configure;

    // time-tunnel related
    private String routers;
	private String user;
	private String pass;
	private String topic;
    private boolean compress;
    private boolean sequence;
    private int timeout;
    private int max_received_size;

    // zonda related
    private int max_buffer_size;
    private int max_flush_size;
    private String log_base_dir;
    private String log_tag;
    private int max_log_file_size;
    private int flush_time;

    static {
        logger.info("loading distribute log server config file: server.properties");
        InputStream in = ClassLoader.getSystemResourceAsStream("server.properties");
        Properties properties = new Properties();
        instance();

        try {
            properties.load(in);
            configure.setRouters(properties.getProperty("tt.routers"));
            configure.setUser(properties.getProperty("tt.user"));
            configure.setPass(properties.getProperty("tt.pass"));
            configure.setTopic(properties.getProperty("tt.topic"));
            configure.setCompress(Boolean.valueOf(properties.getProperty("tt.compress")));
            configure.setSequence(Boolean.valueOf(properties.getProperty("tt.sequence")));
            configure.setTimeout(Integer.parseInt(properties.getProperty("tt.timeout")));
            configure.setMax_received_size(Integer.parseInt(properties.getProperty("tt.max_received_size")));

            configure.setMax_buffer_size(Integer.parseInt(properties.getProperty("zonda.max_buffer_size")));
            configure.setMax_flush_size(Integer.parseInt(properties.getProperty("zonda.max_flush_size")));
            configure.setMax_log_file_size(Integer.parseInt(properties.getProperty("zonda.max_log_file_size")));
            configure.setLog_base_dir(properties.getProperty("zonda.log_base_dir"));
            configure.setLog_tag(properties.getProperty("zonda.log_tag"));
            configure.setFlush_time(Integer.parseInt(properties.getProperty("zonda.flush_time")));

            logger.info("routers: {}", configure.routers);
            logger.info("user: {}", configure.user);
            logger.info("pass: {}", configure.pass);
            logger.info("topic: {}", configure.topic);
            logger.info("compress: {}", configure.compress);
            logger.info("sequence: {}", configure.sequence);
            logger.info("timeout: {}", configure.timeout);
            logger.info("max_received_size: {}", configure.max_received_size);
            logger.info("max_buffer_size: {}", configure.max_buffer_size);
            logger.info("max_flush_size: {}", configure.max_flush_size);
            logger.info("max_log_file_size: {}", configure.max_log_file_size);
            logger.info("log_base_dir: {}", configure.log_base_dir);
            logger.info("log_tag: {}", configure.log_tag);
            logger.info("flush_time: {}", configure.flush_time);

        } catch (Exception e) {
            logger.error("load server.properties failed: {}", e);
            System.exit(-1);
        }
    }

    public synchronized static Configure instance() {
        if (null == configure) {
            configure = new Configure();
        }
        return configure;
    }

    public static void setConfigure(Configure configure) {
        Configure.configure = configure;
    }

    public String getRouters() {
        return routers;
    }

    public void setRouters(String routers) {
        this.routers = routers;
    }

    public String getUser() {
        return user;
    }

    public void setUser(String user) {
        this.user = user;
    }

    public String getPass() {
        return pass;
    }

    public void setPass(String pass) {
        this.pass = pass;
    }

    public String getTopic() {
        return topic;
    }

    public void setTopic(String topic) {
        this.topic = topic;
    }

    public boolean isCompress() {
        return compress;
    }

    public void setCompress(boolean compress) {
        this.compress = compress;
    }

    public boolean isSequence() {
        return sequence;
    }

    public void setSequence(boolean sequence) {
        this.sequence = sequence;
    }

    public int getTimeout() {
        return timeout;
    }

    public void setTimeout(int timeout) {
        this.timeout = timeout;
    }

    public int getMax_received_size() {
        return max_received_size;
    }

    public void setMax_received_size(int max_received_size) {
        this.max_received_size = max_received_size;
    }

    public int getMax_buffer_size() {
        return max_buffer_size;
    }

    public void setMax_buffer_size(int max_buffer_size) {
        this.max_buffer_size = max_buffer_size;
    }

    public int getMax_flush_size() {
        return max_flush_size;
    }

    public void setMax_flush_size(int max_flush_size) {
        this.max_flush_size = max_flush_size;
    }

    public String getLog_base_dir() {
        return log_base_dir;
    }

    public void setLog_base_dir(String log_base_dir) {
        this.log_base_dir = log_base_dir;
    }

    public String getLog_tag() {
        return log_tag;
    }

    public void setLog_tag(String log_tag) {
        this.log_tag = log_tag;
    }

    public int getMax_log_file_size() {
        return max_log_file_size;
    }

    public void setMax_log_file_size(int max_log_file_size) {
        this.max_log_file_size = max_log_file_size;
    }

    public int getFlush_time() {
        return flush_time;
    }

    public void setFlush_time(int flush_time) {
        this.flush_time = flush_time;
    }
}
