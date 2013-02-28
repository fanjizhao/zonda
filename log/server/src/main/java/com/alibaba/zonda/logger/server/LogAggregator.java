/**
 * Project: zonda.logger.server
 * 
 * File Created at 2011-12-17
 * $Id: LogAggregator.java 2011-12-17 $
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

import com.alibaba.zonda.logger.server.log.LogComparator;
import com.alibaba.zonda.logger.server.service.StoppableService;
import com.alibaba.zonda.logger.server.service.flusher.LogFlusher;
import com.alibaba.zonda.logger.server.service.loader.LogLoader;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.*;

/**
 * User: darwin
 * Date: 11-12-17
 * Time: 3:47 pm
 */
public class LogAggregator {

    private Logger logger = LoggerFactory.getLogger(getClass());
    private final List<StoppableService> services;
    private ZondaSortedSet workSet;

    public LogAggregator() {
        this.services = new ArrayList<StoppableService>(2);
        this.workSet = new ZondaSortedSet(new LogComparator());
        this.services.add(new LogLoader(workSet));
        this.services.add(new LogFlusher(workSet));
    }

    public void start() {
        for (StoppableService service : services) {
            service.start();
        }
    }

    public Runnable createShutdownHook() {
        return new ShutdownJob();
    }

    private void destroy() {
        for (StoppableService service : services) {
            service.stop();
        }
    }

    class ShutdownJob implements Runnable {
        public void run() {
            destroy();
        }
    }

    public static void main(String[] args) {
        LogAggregator aggregator = new LogAggregator();
        Runtime.getRuntime().addShutdownHook(new Thread(aggregator.createShutdownHook(), "shutdown hook"));
        aggregator.start();
    }

}
