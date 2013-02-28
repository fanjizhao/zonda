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

import java.text.SimpleDateFormat;

import org.apache.log4j.PatternLayout;
import org.apache.log4j.spi.LocationInfo;
import org.apache.log4j.spi.LoggingEvent;

/**
 * @author darwin.yangq
 */
public class AliPatternLayout extends PatternLayout {
	
	private SimpleDateFormat df = new SimpleDateFormat("yyyyMMdd HH:mm:ss.SSS");

	public String format(LoggingEvent event) {
		
		LocationInfo info = event.getLocationInformation();
		StringBuilder sb = new StringBuilder(1000);
		
		sb.append(df.format(event.timeStamp));
		sb.append(" ");
		sb.append(getSimpleLevelName(event.getLevel().toString()));
		sb.append(" [");
		sb.append(AliLoggerFactory.getHost());
		sb.append(":");
		sb.append(AliLoggerFactory.getModule(event.getLoggerName()));
		sb.append(":");
		sb.append(event.getLogger().getParent().getName());
		sb.append(":");
		sb.append(getSimpleClassName(info.getClassName())); // performance issue
		sb.append(":");
		sb.append(info.getLineNumber()); // performance issue		
		sb.append(":");
		sb.append(info.getMethodName());
		sb.append(":");		
		
		// transaction identifier 
		String str = event.getMessage().toString();
		int i = str.indexOf(TransactionTracer.TP);
		int j = str.indexOf(TransactionTracer.TS);
		if (i != -1 && j != -1) {
			sb.append(str.substring(i + TransactionTracer.TP.length(), j));
			str = str.substring(j + TransactionTracer.TS.length());
		} else {
			sb.append("-");
		}
		
		sb.append("] - ");
		sb.append(str);
		sb.append("\n");		
		return sb.toString();
	}
	
	private String getSimpleLevelName(String level) {
		return level.substring(0, 1);
	}

	private String getSimpleClassName(String name) {
		if (null != name) {
			int index = name.lastIndexOf('.');
			if (index != -1) {
				return name.substring(index + 1);
			} else {
				return name;
			}
		} else {
			return "Default";
		}
	}
	
}
