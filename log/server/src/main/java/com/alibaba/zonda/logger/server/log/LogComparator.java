/**
 * Project: zonda.logger.server
 *
 * File Created at 12-2-3
 * : LogComparator.java 12-2-3 darwin $
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
package com.alibaba.zonda.logger.server.log;

import java.util.Comparator;

/**
 * User: darwin
 * Date: 12-2-3
 * Time: 4:16 pm
 */
public class LogComparator implements Comparator<String> {

    public int compare(String o1, String o2) {
        if (null != o1 && null != o2) {
            if (o1.length() > 24 && o2.length() > 24) {
                return o1.substring(0, 24).compareTo(o2.substring(0, 24));
            } else if (o1.length() > 24) {
                return 1;
            } else {
                return -1;
            }
        } else if (null != o1) {
            return 1;
        } else {
            return -1;
        }
    }

}
