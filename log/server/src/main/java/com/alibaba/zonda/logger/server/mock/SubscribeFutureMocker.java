/**
 * Project: zonda.logger.server
 *
 * File Created at 12-2-3
 * : SubscribeFutureMocker.java 12-2-3 darwin $
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
package com.alibaba.zonda.logger.server.mock;

import com.taobao.timetunnel.client.Message;
import com.taobao.timetunnel.client.SubscribeFuture;
import com.taobao.timetunnel.client.util.ClosedException;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.TimeUnit;

/**
 * User: darwin
 * Date: 12-2-3
 * Time: 5:31 pm
 */
public class SubscribeFutureMocker implements SubscribeFuture {

    private SimpleDateFormat df = new SimpleDateFormat("yyyyMMdd HH:mm:ss.SSS");
    private boolean active = true;
    private int counter = 1;

    public SubscribeFutureMocker() throws ClosedException {

    }

    public List<Message> get() {
        if (!active) {
            return new ArrayList<Message>(0);
        }

        List<Message> msgs = new ArrayList<Message>(5);
        for (int i = 0; i < 1234; i++) {
            msgs.add(new MessageMocker(df.format(System.currentTimeMillis()) + "000 I [test] log test message id " + (counter++) ));
        }
        return msgs;
    }

    public List<Message> get(long l, TimeUnit timeUnit) {
        return get();
    }

    public void cancel() {
        this.active = false;
    }
}
