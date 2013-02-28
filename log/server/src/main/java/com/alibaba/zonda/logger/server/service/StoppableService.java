/**
 * Project: zonda.logger.server
 *
 * File Created at 12-2-3
 * StoppableService: StoppableService.java 12-2-3 darwin $
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
package com.alibaba.zonda.logger.server.service;

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * User: darwin
 * Date: 12-2-3
 * Time: 10:52 am
 */
public abstract class StoppableService implements Runnable {

    private static final int STOP_TIMEOUT_IN_SECONDS = 240;
    private AtomicBoolean stopRequested = new AtomicBoolean(false);
    private CountDownLatch stoppedSignal = new CountDownLatch(1);

    public void start() {
        System.out.println("starting up...");
        new Thread(this).start();
    }

    public void stop() {
        System.out.println("stop pending...");
        stopRequested.set(true);
        shutdown();
        try {
            stoppedSignal.await(STOP_TIMEOUT_IN_SECONDS, TimeUnit.SECONDS);
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
        System.out.println("finish stopping...");
    }    

    public void run() {
        prepare();
        while (!stopRequested.get()) {
            try {
                execute();
            } catch (Throwable e) {
                System.err.println("execution error");
                break;
            }
        }
        stoppedSignal.countDown();
        System.out.println("main process ended");
        if (!stopRequested.get()) {
            System.exit(-1);
        }
    }

    public abstract void prepare();

    public abstract void execute();

    public abstract void shutdown();

}
