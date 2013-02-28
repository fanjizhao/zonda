/**
 * Project: zonda.logger.server
 *
 * File Created at 12-2-5
 * : LogServer.java 12-2-5 darwin $
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

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * User: darwin
 * Date: 12-2-5
 * Time: 4:24 pm
 */
public class LogServer {

    private static Logger logger = LoggerFactory.getLogger(LogServer.class);

    public static void main(String[] args) {

        String application = "monitor";
        String arguments = "";
        
        if (null == args || args.length < 1) {
            logger.info("run default application: monitor");
        } else {
            logger.info("run application: {}", args[0]);
            application = args[0];
        }

        if ("aggregator".equals(application)) {
            LogAggregator aggregator = new LogAggregator();
            Runtime.getRuntime().addShutdownHook(new Thread(aggregator.createShutdownHook(), "shutdown hook"));
            aggregator.start();
        } else {
            LogMonitor monitor = new LogMonitor();
            monitor.start();
        }
    }

}
