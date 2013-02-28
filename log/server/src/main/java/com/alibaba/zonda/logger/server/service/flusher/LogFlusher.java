/**
 * Project: zonda.logger.server
 *
 * File Created at 12-2-2
 * LogFlusher: LogFlusher.java 12-2-2 darwin $
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
package com.alibaba.zonda.logger.server.service.flusher;

import com.alibaba.zonda.logger.server.ZondaSortedSet;
import com.alibaba.zonda.logger.server.config.Configure;
import com.alibaba.zonda.logger.server.service.StoppableService;
import com.alibaba.zonda.logger.server.writer.OutputStreamManager;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.util.*;

/**
 * User: darwin
 * Date: 12-2-2
 * Time: 5:52 pm
 */
public class LogFlusher extends StoppableService {

    private final Logger logger = LoggerFactory.getLogger(getClass());
    private OutputStreamManager osManager = OutputStreamManager.getInstance();
    private ZondaSortedSet workSet;
    private Configure configure;

    public LogFlusher(ZondaSortedSet workSet) {
        this.configure = Configure.instance();
        this.workSet = workSet;
    }

    public void prepare() {
        Thread.setDefaultUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
            public void uncaughtException(Thread t, Throwable e) {
                System.err.println(e.getMessage());
                System.exit(-1);
            }
        });
    }

    public void execute() {
        while (true) {
            try {
                Thread.sleep(configure.getFlush_time() * 1000);
                synchronized (workSet) {
                    for (int number = 0; number < configure.getMax_flush_size() && number < workSet.size(); number++) {
                        String log = workSet.first();
                        try {
                            osManager.write(log.getBytes(), configure.getLog_tag());
                        } catch (IOException e) {
                            logger.error("flush log: {} failed: {}", log, e);
                        }
                        workSet.remove(log);
                    }
                }
            } catch (InterruptedException e) {
                logger.error("log loader interrupt exception, msg: {}", e);
                System.exit(-1);
            }
        }
    }

    public void shutdown() {
        logger.info("shutdown log loader");
    }
}
