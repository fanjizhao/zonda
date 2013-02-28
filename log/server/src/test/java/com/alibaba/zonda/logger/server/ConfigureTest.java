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
package com.alibaba.zonda.logger.server;

import com.alibaba.zonda.logger.server.config.Configure;
import junit.framework.Assert;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

/**
 * @author darwin.yangq
 */
public class ConfigureTest {
	
	private Configure configuration;

	@Before
	public void setUp() throws Exception {
		configuration = Configure.instance();
	}

	@After
	public void tearDown() throws Exception {
		
	}

	@Test
	public void testGetRouters() {
		Assert.assertEquals(configuration.getRouters(), "10.20.153.134:9090");
	}

	@Test
	public void testGetUser() {
		Assert.assertEquals(configuration.getUser(), "yq2");
	}

	@Test
	public void testGetPass() {
		Assert.assertEquals(configuration.getPass(), "123456");
	}

	@Test
	public void testGetTopic() {
		Assert.assertEquals(configuration.getTopic(), "systemlogs");
	}

}
