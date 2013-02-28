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
package com.alibaba.zonda.logger.watcher.filter;

import java.io.InputStream;
import java.util.Properties;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * @author darwin.yangq
 */
public class Filter {

	private boolean showLogLevel = true;
	
	private String levels = "";

	private boolean showHostname = true;
	
	private String hostnames = "";

	private boolean showModule = true;
	
	private String modules = "";
	
	private boolean showCategory = true;
	
	private String categories = "";

	private boolean showClass = true;
	
	private String classes = "";

	private boolean showMethod = true;
	
	private String methods = "";

	private boolean showLineNumber = true;
	
	private String lineNumbers = "";;
	
	private boolean showTxId = true;
	
	private String txIds = "";
	
	private Logger logger = LoggerFactory.getLogger(getClass());
	
	public Filter() {
		this("log-filter.properties");
	}
	
	public Filter(String config) {
		
		logger.info("loading distribute dfs log filter config file: " + config);
		InputStream in = ClassLoader.getSystemResourceAsStream(config);
		Properties p = new Properties();
		
		try {
			p.load(in);	// load properties key-value into property object
			
			this.showLogLevel = Boolean.valueOf(p.getProperty("showLogLevel"));
			this.showHostname = Boolean.valueOf(p.getProperty("showHostname"));
			this.showModule = Boolean.valueOf(p.getProperty("showModule"));
			this.showMethod = Boolean.valueOf(p.getProperty("showMethod"));
			this.showLineNumber = Boolean.valueOf(p.getProperty("showLineNumber"));
			this.showTxId = Boolean.valueOf(p.getProperty("showTxId"));
			
			this.levels = p.getProperty("levels");
			this.lineNumbers = p.getProperty("lineNumbers");
			this.modules = p.getProperty("modules");
			this.methods = p.getProperty("methods");
			this.txIds = p.getProperty("txIds");
			this.hostnames = p.getProperty("hostnames");
			
			logger.info("showLogLevel: " + showLogLevel);
			logger.info("showHostname: " + showHostname);
			logger.info("showModule: " + showModule);
			logger.info("showMethod: " + showMethod);
			logger.info("showLineNumber: " + showLineNumber);
			logger.info("showTxId: " + showTxId);
			
			logger.info("levels: " + levels);
			logger.info("lineNumbers: " + lineNumbers);
			logger.info("modules: " + modules);
			logger.info("methods: " + methods);
			logger.info("txIds: " + txIds);
			logger.info("hostnames: " + hostnames);
			
		} catch (Exception e) {
			logger.error("load log-server.properties failed, " + e);
		}	
	}

	public boolean isShowLogLevel() {
		return showLogLevel;
	}

	public void setShowLogLevel(boolean showLogLevel) {
		logger.info("change showLogLevel: " + showLogLevel);
		this.showLogLevel = showLogLevel;
	}

	public boolean isShowHostname() {
		return showHostname;
	}

	public void setShowHostname(boolean showHostname) {
		logger.info("change showHostname: " + showHostname);
		this.showHostname = showHostname;
	}

	public boolean isShowModule() {
		return showModule;
	}

	public void setShowModule(boolean showModule) {
		logger.info("change showModule: " + showModule);
		this.showModule = showModule;
	}

	public boolean isShowClass() {
		return showClass;
	}

	public void setShowClass(boolean showClass) {
		logger.info("change showClass: " + showClass);
		this.showClass = showClass;
	}

	public boolean isShowMethod() {
		return showMethod;
	}

	public void setShowMethod(boolean showMethod) {
		logger.info("change showMethod: " + showMethod);
		this.showMethod = showMethod;
	}

	public boolean isShowLineNumber() {
		return showLineNumber;
	}

	public void setShowLineNumber(boolean showLineNumber) {
		logger.info("change showLineNumber: " + showLineNumber);
		this.showLineNumber = showLineNumber;
	}

	public String getHostnames() {
		return hostnames;
	}

	public void setHostnames(String hostnames) {
		logger.info("change hostnames: " + hostnames);
		this.hostnames = hostnames;
	}

	public String getModules() {
		return modules;
	}

	public void setModules(String modules) {
		logger.info("change modules: " + modules);
		this.modules = modules;
	}

	public String getClasses() {
		return classes;
	}

	public void setClasses(String classes) {
		logger.info("change classes: " + classes);
		this.classes = classes;
	}

	public String getMethods() {
		return methods;
	}

	public void setMethods(String methods) {
		logger.info("change methods: " + methods);
		this.methods = methods;
	}

	public String getLineNumbers() {
		return lineNumbers;
	}

	public void setLineNumbers(String lineNumbers) {
		logger.info("change lineNumbers: " + lineNumbers);
		this.lineNumbers = lineNumbers;
	}

	public boolean isShowTxId() {
		return showTxId;
	}

	public void setShowTxId(boolean showTxId) {
		logger.info("change showTxId: " + showTxId);
		this.showTxId = showTxId;
	}

	public String getTxIds() {
		return txIds;
	}

	public void setTxIds(String txIds) {
		logger.info("change txIds: " + txIds);
		this.txIds = txIds;
	}

	public String getLevels() {
		return levels;
	}

	public void setLevels(String levels) {
		logger.info("change levels: " + levels);
		this.levels = levels;
	}

	public boolean isShowCategory() {
		return showCategory;
	}

	public void setShowCategory(boolean showCategory) {
		logger.info("change showCategory: " + showCategory);
		this.showCategory = showCategory;
	}

	public String getCategories() {
		return categories;
	}

	public void setCategories(String categories) {
		logger.info("change categories: " + categories);
		this.categories = categories;
	}

}
