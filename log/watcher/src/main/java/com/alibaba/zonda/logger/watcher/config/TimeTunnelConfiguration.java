/**
 * Project: zonda.logger.server
 * 
 * File Created at 2011-12-17
 * $Id: ArandaQueueListener.java 29442 2009-12-08 12:21:36Z shuo.qius $
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
package com.alibaba.zonda.logger.watcher.config;

import java.io.InputStream;
import java.util.Properties;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * @author darwin.yangq
 */
public class TimeTunnelConfiguration {

	private String routers;

	private String user;

	private String pass;

	private String topic;
	
	private Logger logger = LoggerFactory.getLogger(getClass());
	
	public TimeTunnelConfiguration(String config) {
		
		logger.info("loading distribute dfs server config file: " + config);
		InputStream in = ClassLoader.getSystemResourceAsStream(config);
		Properties p = new Properties();
		
		try {
			p.load(in);	// load properties key-value into property object
			this.setRouters(p.getProperty("routers"));
			this.setUser(p.getProperty("user"));
			this.setPass(p.getProperty("pass"));
			this.setTopic(p.getProperty("topic"));
			
			logger.info("routers: " + this.routers);
			logger.info("user: " + this.user);
			logger.info("pass: " + this.pass);
			logger.info("topic: " + this.topic);
			
		} catch (Exception e) {
			logger.error("load log-server.properties failed, " + e);
		}		
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

}
