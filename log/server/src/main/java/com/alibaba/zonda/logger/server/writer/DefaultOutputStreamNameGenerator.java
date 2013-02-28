/**
 * Project: zonda.logger.server
 *
 * File Created at 12-2-3
 * : DefaultOutputStreamNameGenerator.java 12-2-3 darwin $
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
package com.alibaba.zonda.logger.server.writer;

import com.alibaba.zonda.logger.server.util.DateUtil;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.UUID;

/**
 * User: darwin
 * Date: 12-2-3
 * Time: 1:57 pm
 */
public class DefaultOutputStreamNameGenerator implements OutputStreamNameGenerator {

    private Logger logger = LoggerFactory.getLogger(getClass());

    public String generateFileName(String baseDir, String tag) {
        StringBuilder sb = new StringBuilder();
        sb.append(baseDir);
        sb.append("/");
        sb.append(tag);
        sb.append(DateUtil.getTimeStampInMin('_', null));
        sb.append("#");
        sb.append(UUID.randomUUID().toString());
        sb.append(".tmp");
        logger.debug("generate log file: {}", sb.toString());
        return sb.toString();
    }

}
