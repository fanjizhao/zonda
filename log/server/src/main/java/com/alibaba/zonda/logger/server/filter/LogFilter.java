/**
 * Project: zonda.logger.server
 *
 * File Created at 2011-12-17
 * $Id: LogFilter.java 2011-12-17 $
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
package com.alibaba.zonda.logger.server.filter;

import java.awt.datatransfer.StringSelection;
import java.io.InputStream;
import java.util.Properties;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * User: darwin
 * Date: 11-12-17
 * Time: 3:47 pm
 */
public class LogFilter {

    private static Logger logger = LoggerFactory.getLogger(LogFilter.class);

    private static LogFilter logFilter;

    private String levels;

    private boolean showHost;
    
    private String hosts;

    private boolean showModule;

    private String modules;
    
    private boolean showCategory;
    
    private String categories;

    private boolean showFile;
    
    private String files;

    private boolean showMethod;
    
    private String methods;

    private String txCodes;
    
    private boolean showThreadCode;
    
    private String threadCodes;
    
    private String contents;

    static {
        logger.info("loading distribute log server config file: filter.properties");
        InputStream in = ClassLoader.getSystemResourceAsStream("filter.properties");
        Properties properties = new Properties();
        instance();

        try {
            properties.load(in);
            logFilter.setLevels(properties.getProperty("levels"));
            logFilter.setShowHost(Boolean.valueOf(properties.getProperty("showHost")));
            logFilter.setHosts(properties.getProperty("hosts"));
            logFilter.setShowModule(Boolean.valueOf(properties.getProperty("showModule")));
            logFilter.setModules(properties.getProperty("modules"));
            logFilter.setShowCategory(Boolean.valueOf(properties.getProperty("showCategory")));
            logFilter.setCategories(properties.getProperty("categories"));
            logFilter.setShowFile(Boolean.valueOf(properties.getProperty("showFile")));
            logFilter.setFiles(properties.getProperty("files"));
            logFilter.setMethods(properties.getProperty("methods"));
            logFilter.setTxCodes(properties.getProperty("txCodes"));
            logFilter.setShowThreadCode(Boolean.valueOf(properties.getProperty("showThreadCode")));
            logFilter.setThreadCodes(properties.getProperty("threadCodes"));
            logFilter.setContents(properties.getProperty("contents"));

            logger.info("levels: {}", logFilter.levels);
            logger.info("show host: {}", logFilter.showHost);
            logger.info("hosts: {}", logFilter.hosts);
            logger.info("show module: {}", logFilter.showModule);
            logger.info("modules: {}", logFilter.modules);
            logger.info("showCategory: {}", logFilter.showCategory);
            logger.info("categories: {}", logFilter.categories);
            logger.info("files: {}", logFilter.files);
            logger.info("methods: {}", logFilter.methods);
            logger.info("txCodes: {}", logFilter.txCodes);
            logger.info("showThreadCode: {}", logFilter.showThreadCode);
            logger.info("threadCodes: {}", logFilter.threadCodes);
            logger.info("contents: {}", logFilter.contents);

        } catch (Exception e) {
            logger.error("load filter.properties failed: {}", e);
            System.exit(-1);
        }
    }

    public synchronized static LogFilter instance() {
        if (null == logFilter) {
            logFilter = new LogFilter();
        }
        return logFilter;
    }

    private LogFilter() {

    }

    public boolean match(String log) {
        // 20120204 12:55:55.123000 I [host:module:category:file:112:method:txCode:threadCode] content
        boolean matched = true;
        int start = 25;
        int end = log.indexOf("]");
        if (end == -1) {
            return matched;
        }

        String meta = log.substring(start, end); // I [host:module:category:file:112:method:txCode:threadCode
        String level = meta.substring(0, 1);
        String[] strs = meta.substring(3).split(":");
        if (strs.length < 8) {
            return false;
        }

        String host = strs[0];
        String module = strs[1];
        String category = strs[2];
        String file = strs[3];
        String method = strs[5];
        String txCode = strs[6];
        String threadCode = strs[7];
        String content = log.substring(end + 1);

        logger.debug("host: {}", host);
        logger.debug("module: {}", module);
        logger.debug("category: {}", category);
        logger.debug("file: {}", file);
        logger.debug("method: {}", method);
        logger.debug("txCode: {}", txCode);
        logger.debug("threadCode: {}", threadCode);

        if (null != levels && !"".equals(levels)) {
            matched &= this.levels.indexOf(level) != -1;{
                matched &= this.methods.indexOf(method) != -1;
            }
        }
        if (null != hosts && !"".equals(hosts)) {
            matched &= this.hosts.indexOf(host) != -1;
        }
        if (null != modules && !"".equals(modules)) {
            matched &= this.modules.indexOf(module) != -1;
        }
        if (null != categories && !"".equals(categories)) {
            matched &= this.categories.indexOf(category) != -1;
        }
        if (null != files && !"".equals(files)) {
            matched &= this.files.indexOf(file) != -1;
        }
        if (null != methods && !"".equals(methods))
        if (null != txCodes && !"".equals(txCodes)) {
            matched &= this.txCodes.indexOf(txCode) != -1;
        }
        if (null != threadCodes && !"".equals(threadCodes)) {
            matched &= this.threadCodes.indexOf(threadCode) != -1;
        }
        if (null != contents && !"".equals(contents)) {
            String[] cs = this.contents.split(",");
            for (String c : cs) {
                matched &= content.indexOf(c) != -1;
                if (matched) {
                    return true;
                }
            }
            matched = false;
        }

        return matched;
    }
    
    public String getLevels() {
        return levels;
    }

    public void setLevels(String levels) {
        this.levels = levels;
    }

    public boolean isShowHost() {
        return showHost;
    }

    public void setShowHost(boolean showHost) {
        this.showHost = showHost;
    }

    public String getHosts() {
        return hosts;
    }

    public void setHosts(String hosts) {
        this.hosts = hosts;
    }

    public boolean isShowModule() {
        return showModule;
    }

    public void setShowModule(boolean showModule) {
        this.showModule = showModule;
    }

    public String getModules() {
        return modules;
    }

    public void setModules(String modules) {
        this.modules = modules;
    }

    public boolean isShowCategory() {
        return showCategory;
    }

    public void setShowCategory(boolean showCategory) {
        this.showCategory = showCategory;
    }

    public String getCategories() {
        return categories;
    }

    public void setCategories(String categories) {
        this.categories = categories;
    }

    public boolean isShowFile() {
        return showFile;
    }

    public void setShowFile(boolean showFile) {
        this.showFile = showFile;
    }

    public String getFiles() {
        return files;
    }

    public void setFiles(String files) {
        this.files = files;
    }

    public boolean isShowMethod() {
        return showMethod;
    }

    public void setShowMethod(boolean showMethod) {
        this.showMethod = showMethod;
    }

    public String getMethods() {
        return methods;
    }

    public void setMethods(String methods) {
        this.methods = methods;
    }

    public String getTxCodes() {
        return txCodes;
    }

    public void setTxCodes(String txCodes) {
        this.txCodes = txCodes;
    }

    public boolean isShowThreadCode() {
        return showThreadCode;
    }

    public void setShowThreadCode(boolean showThreadCode) {
        this.showThreadCode = showThreadCode;
    }

    public String getThreadCodes() {
        return threadCodes;
    }

    public void setThreadCodes(String threadCodes) {
        this.threadCodes = threadCodes;
    }

    public String getContents() {
        return contents;
    }

    public void setContents(String contents) {
        this.contents = contents;
    }
}
