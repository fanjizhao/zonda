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

/**
 * @author darwin.yangq
 */
public interface TransactionTracer {
	
	public static final String TP = "__TX=";

	public static final String TS = "__ ";
	
	public static final String TX = TP + "{}" + TS;
	
}
