/**
 * Project: zonda.logger.server
 *
 * File Created at 12-2-3
 * : ZondaSortedSet.java 12-2-3 darwin $
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

import java.util.Comparator;
import java.util.SortedSet;
import java.util.concurrent.ConcurrentSkipListSet;

/**
 * User: darwin
 * Date: 12-2-3
 * Time: 6:36 pm
 */
public class ZondaSortedSet extends ConcurrentSkipListSet<String> {

    private Configure configure;

    public ZondaSortedSet(Comparator<String> comparator) {
        super(comparator);
        this.configure = Configure.instance();
    }

    public int getCapacity() {
        return this.configure.getMax_buffer_size();
    }
    
    public boolean hasFreeSpace() {
        return (configure.getMax_buffer_size() - size()) >= configure.getMax_flush_size();
    }
    
}
