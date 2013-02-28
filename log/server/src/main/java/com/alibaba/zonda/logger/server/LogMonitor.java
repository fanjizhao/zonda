/**
 * Project: zonda.logger.server
 *
 * File Created at 12-2-5
 * : LogMonitor.java 12-2-5 darwin $
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
import com.alibaba.zonda.logger.server.filter.LogFilter;
import org.apache.commons.io.IOUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.*;
import java.util.List;

/**
 * User: darwin
 * Date: 12-2-5
 * Time: 12:02 pm
 */
public class LogMonitor extends Thread {

    private Logger logger = LoggerFactory.getLogger(getClass());
    
    private LogFilter logFilter = LogFilter.instance();

    private Configure configure = Configure.instance();

    private File baseDir = new File(configure.getLog_base_dir());

    private FileFilter fileFilter = new FileFilter() {
        public boolean accept(File path) {
            return path.getName().startsWith(configure.getLog_tag()) && path.getName().endsWith(".tmp");
        }
    };
    
    private File logFile;
    
    private InputStream stream;
    
    private OutputStream out = System.out;

    private long modifyTime = 0;
    
    private int lineNo = 0;
    
    private int checkInterval = 1000;
    
    private int consoleSize;

    public LogMonitor() {
        logger.info("monitor base dir: {}", configure.getLog_base_dir());
        logger.info("monitor log file: {}", logFile);
        logger.info("last modify time: {}", modifyTime);
        logger.info("last access line: {}", lineNo);
        logger.info("monitor interval: {}", checkInterval);
        logger.info("std console size: {}", consoleSize);
    }

    private void refresh() {

        // select one log file randomly
        // maybe the first time
        if (null == logFile) {
            File[] logs = baseDir.listFiles(fileFilter);
            if (null != logs && logs.length > 0) {
                try {
                    this.logFile = logs[0];
                    this.stream = new BufferedInputStream(new FileInputStream(logs[0]), 8192);
                } catch (FileNotFoundException e) {
                    logger.error("log file not exists, try later");
                    return;
                }
            }
        }

        long nowModified = logFile.lastModified();
        if (nowModified < modifyTime) {
            logger.debug("current log file: {} is running, modify time: {}", logFile, nowModified);
            return;
        } else {
            logger.debug("current log file: {} is silent, refresh directory", logFile);
            logger.debug("current log modify time: {}", this.modifyTime);
            logger.debug("last modify time: {}", nowModified);
            logger.debug("last line number: {}", lineNo);
            this.modifyTime = nowModified;
        }

        try {
            File[] logs = baseDir.listFiles(fileFilter);
            if (null != logs) { // select the latest modified xxx.tmp log file
                for (File log : logs) {
                    if ( (log.lastModified() > modifyTime) && !(log.getName().equals(logFile.getName())) ) {
                        logger.info("using new log file: {}", log.getName());
                        if (null != this.stream) {
                            this.stream.close();
                        }

                        this.logFile = log;
                        this.stream = new BufferedInputStream(new FileInputStream(log), 8192);
                        this.lineNo = 0;
                    }
                }
            }
        } catch (Exception e) {
            logger.error("list log file failed, message: {}", e);
        }
    }

    private void tail() throws IOException {
        logger.debug("current log file: {}", logFile);
        logger.debug("current line no: {}", lineNo);

        if (null != logFile && null != stream) {
            List<String> logs = IOUtils.readLines(stream, "utf-8");
            for (String log : logs) {
                if (!log.trim().equals("") && logFilter.match(log)) {
                    this.out.write(log.getBytes());
                    this.out.write("\n".getBytes());
                }
            }
            this.lineNo += logs.size();
            this.out.flush();
        }
    }

    public void run() {
        while (true) {
            try {
                this.refresh(); // test whether current log is still running
                this.tail(); // show the incremental log messages
                Thread.sleep(checkInterval);
            }
            catch (Exception e) {
                logger.error("log monitor failed, error: {}", e);
            }
        }
    }

    public static void main(String[] args) {
        LogMonitor monitor = new LogMonitor();
        monitor.start();
    }

}
