/**
 * Project: zonda.logger.lib
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
package com.alibaba.platform.zonda.logger.lib;

import java.net.InetAddress;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * @author darwin.yangq
 */
public class AliLoggerFactory {
	
	private static Map<String, String> map = new ConcurrentHashMap<String, String>(100);
	private static String host = "127.0.0.1";
	private static String module = "default";	
	
	static {
		try {
			InetAddress addr = InetAddress.getLocalHost();
			AliLoggerFactory.host = addr.getHostName().toString();		
		} catch (Exception e) {
			System.err.println("query host name or ip failed: " + e);
		}
	}

	public static Logger getLogger(String module, Class<?> clazz) {
		Logger logger = LoggerFactory.getLogger(clazz);
		map.put(clazz.getName(), module);
		return logger;
	}
	
	public static Logger getLogger(Class<?> clazz) {
		Logger logger = LoggerFactory.getLogger(clazz);
		return logger;
	}
	
	public static void setHost(String host) {
		AliLoggerFactory.host = host;
	}
	
	public static String getHost() {
		return AliLoggerFactory.host;
	}
	
	public static String getModule(String clazz) {
		String module = AliLoggerFactory.map.get(clazz);
		if (null == module) {
			module = AliLoggerFactory.module;
		}
		return module;
	}
	
	public static void setModule(String module) {
		AliLoggerFactory.module = module;
	}
	
}
