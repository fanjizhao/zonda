/**
 * Project: zonda.logger.server
 *
 * File Created at 12-2-3
 * LogLoader: LogLoader.java 12-2-3 darwin $
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
package com.alibaba.zonda.logger.server.service.loader;

import com.alibaba.zonda.logger.server.ZondaSortedSet;
import com.alibaba.zonda.logger.server.config.Configure;
import com.alibaba.zonda.logger.server.mock.SubscribeFutureMocker;
import com.alibaba.zonda.logger.server.service.StoppableService;
import com.taobao.timetunnel.client.Message;
import com.taobao.timetunnel.client.SubscribeFuture;
import com.taobao.timetunnel.client.util.ClosedException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.SortedSet;
import java.util.concurrent.TimeUnit;

import static com.taobao.timetunnel.client.TimeTunnel.*;

/**
 * User: darwin
 * Date: 12-2-3
 * Time: 10:48 pm
 */
public class LogLoader extends StoppableService {

    private final Logger logger = LoggerFactory.getLogger(getClass());
    private ZondaSortedSet workSet;
    private SubscribeFuture subscriber;
    private Configure configure;
    private boolean connected = false;

    public LogLoader(ZondaSortedSet workSet) {
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

    private void connectTimeTunnel() {
        try {
            com.taobao.timetunnel.client.impl.Config.getInstance().setRouterServerList(configure.getRouters());
            com.taobao.timetunnel.client.impl.Config.getInstance().setAppName("dfs");
            use(passport(configure.getUser(), configure.getPass()));

            //subscriber = subscribe(tunnel(configure.getTopic(), configure.isCompress(), configure.isSequence(),
            //configure.getTimeout(), configure.getMax_received_size()));

            this.subscriber = new SubscribeFutureMocker();
            this.connected = true;

        } catch (ClosedException e) {
            logger.error("close has been called");
            System.exit(-1);
        }
    }

    private void closeTimeTunnel() {
        this.subscriber.cancel();
        this.connected = false;
    }

    public void execute() {
        try {
            if (this.connected) {
                logger.info("time tunnel connection is running and buffer size is: {}", workSet.size());
                List<Message> ms = subscriber.get(configure.getTimeout(), TimeUnit.SECONDS);
                process(ms);
            }
            else if (this.workSet.hasFreeSpace()) {
                logger.info("zonda log buffer has free space, connect to time tunnel again");
                this.connectTimeTunnel();
            }
            else {
                //logger.info("zonda log buffer is still full, try to test later");
            }
        } catch (IOException e) {
            logger.error("log loader error: {}", e.getMessage());
            throw new RuntimeException(e);
        }
    }

    private void process(List<Message> messages) throws IOException {
        logger.info("process message list, size: {}", messages.size());
        logger.info("zonda log buffer size: {}", workSet.size());

        for (Iterator<Message> it = messages.iterator(); it.hasNext();) {
            String str = asString(it.next());
            List<String> tempSet = Arrays.asList(str.split("\n"));

            // logger.info("work set size: {}", workSet.size());
            // logger.info("temp set size: {}", tempSet.size());
            // logger.info("work set + temp set size: {}", workSet.size() + tempSet.size());

            // close tt connection
            if (workSet.size() + tempSet.size() > configure.getMax_buffer_size()) {
                this.closeTimeTunnel();
            }

            // insert sort algorithm
            for (String temp : tempSet) {
                workSet.add(temp + "\n");
            }
        }
    }

    public void shutdown() {
        logger.info("shutdown receive future");
        subscriber.cancel();
    }
}
