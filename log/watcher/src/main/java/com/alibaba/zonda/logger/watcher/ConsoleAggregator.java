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
package com.alibaba.zonda.logger.watcher;

import static com.taobao.timetunnel.client.TimeTunnel.asString;
import static com.taobao.timetunnel.client.TimeTunnel.passport;
import static com.taobao.timetunnel.client.TimeTunnel.subscribe;
import static com.taobao.timetunnel.client.TimeTunnel.tunnel;
import static com.taobao.timetunnel.client.TimeTunnel.use;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.alibaba.zonda.logger.watcher.config.TimeTunnelConfiguration;
import com.alibaba.zonda.logger.watcher.filter.Filter;
import com.taobao.timetunnel.client.Message;
import com.taobao.timetunnel.client.SubscribeFuture;
import com.taobao.timetunnel.client.impl.Config;
import com.taobao.timetunnel.client.util.ClosedException;

/**
 * @author darwin.yangq
 */
public class ConsoleAggregator {

	private TimeTunnelConfiguration config;

	private Filter filter;
	
	private Logger logger = LoggerFactory.getLogger(getClass());

	public ConsoleAggregator(TimeTunnelConfiguration config, Filter filter) {
		this.config = config;
		this.filter = filter;
	}

	public void setConfiguration(TimeTunnelConfiguration config) {
		this.config = config;
	}

	public void setFilter(Filter filter) {
		this.filter = filter;
	}
	
	public void execute() {
		
		Config.getInstance().setRouterServerList(this.config.getRouters());
		Config.getInstance().setAppName("console");
		use(passport(this.config.getUser(), this.config.getPass()));
		SubscribeFuture subscriber = null;
		
		try {
			subscriber = subscribe(tunnel(this.config.getTopic()));
		} catch (ClosedException e) {
			e.printStackTrace();
		}
		
		while (true) {
			List<Message> ms = subscriber.get();
			for (Iterator<Message> it = ms.iterator(); it.hasNext();) {
				showLog(asString(it.next()));
			}
		}
		
		// must not be invoked
		// will not receive new message again except restart broker server
		// subscriber.cancel();
	}
	
	private void showLog(String str) {
		if (null != this.filter) {
			List<LogSegment> segments = convertString2LogSegments(str);
			if (null != segments) {
				for (LogSegment segment : segments) {
					
					if (!this.filter.getLevels().equals("") && !this.filter.getLevels().contains(segment.level)) {
						continue;
					}
					if (!this.filter.getHostnames().equals("") && !this.filter.getHostnames().contains(segment.hostname)) {
						continue;
					}
					if (!this.filter.getModules().equals("") && !this.filter.getModules().contains(segment.module)) {
						continue;
					}
					if (!this.filter.getCategories().equals("") && !this.filter.getCategories().contains(segment.category)) {
						continue;
					}					
					if (!this.filter.getClasses().equals("") && !this.filter.getClasses().contains(segment.clazz)) {
						continue;
					}
					if (!this.filter.getMethods().equals("") && !this.filter.getMethods().contains(segment.method)) {
						continue;
					}
					if (!this.filter.getLineNumbers().equals("") && !this.filter.getLineNumbers().contains(segment.lineNumber)) {
						continue;
					}						
					if (!this.filter.getTxIds().equals("") && !this.filter.getTxIds().contains(segment.txId)) {
						continue;
					}	
					
					StringBuilder sb = new StringBuilder(1000);
					sb.append(segment.timestamp);
					sb.append(" ");
					
					if (this.filter.isShowLogLevel()) {
						sb.append(segment.level);
					}
					
					sb.append(" [");
					
					if (this.filter.isShowHostname()) {
						sb.append(segment.hostname);
						sb.append(":");
					}
					
					if (this.filter.isShowModule()) {
						sb.append(segment.module);
						sb.append(":");
					}
					
					if (this.filter.isShowCategory()) {
						sb.append(segment.category);
						sb.append(":");
					}					
					
					if (this.filter.isShowClass()) {
						sb.append(segment.clazz);
						sb.append(":");
					}
					
					if (this.filter.isShowLineNumber()) {
						sb.append(segment.lineNumber);
						sb.append(":");
					}
					
					if (this.filter.isShowMethod()) {
						sb.append(segment.method);
						sb.append(":");
					}
					
					if (this.filter.isShowTxId()) {
						sb.append(segment.txId);
					}
					
					sb.append("] - ");
					sb.append(segment.msg);
					System.out.println(sb);
				}				
			}
		}
	}
	
	public List<LogSegment> convertString2LogSegments(String str) {
		List<LogSegment> segments = null;
		if (null != str) {
			String[] strs = str.split("\n");
			segments = new ArrayList<LogSegment>(strs.length);
			for (String s : strs) {
				
				LogSegment seg = new LogSegment();
				segments.add(seg);
				
				int start = s.indexOf("[");
				int end = s.indexOf("]");
				
				// parse log meta data
				if (start != -1 && end != -1) {
					
					// outside []
					seg.timestamp = s.substring(0, start - 3);
					seg.level = s.substring(start - 2, start - 1);
					seg.msg = s.substring(end + 4);
					
					// inside []
					String t = s.substring(start + 1, end);
					String[] ts = t.split(":");
					if (ts == null || ts.length != 7) {
						logger.error("invalid log string format: " + t);
					} else {
						seg.hostname = ts[0];
						seg.module = ts[1];
						seg.category = ts[2];
						seg.clazz = ts[3];
						seg.lineNumber = ts[4];
						seg.method = ts[5];
						seg.txId = ts[6];
					}
				}
			}
		} else {
			logger.info("convert string2log: str is null");
		}
		return segments;
	}
	
	public class LogSegment {
		
		public String timestamp = "";
		
		public String level = "";
		
		public String hostname = "";
		
		public String module = "";
		
		public String category = "";
		
		public String clazz = "";
		
		public String method = "";
		
		public String lineNumber = "";
		
		public String txId = "";
		
		public String msg = "";
	
	}

}
