/**
 * Project: zonda.logger.server
 *
 * File Created at 12-2-3
 * : BytesUtil.java 12-2-3 darwin $
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
package com.alibaba.zonda.logger.server.util;

import java.nio.ByteBuffer;

/**
 * User: darwin
 * Date: 12-2-3
 * Time: 2:18 pm
 */
public class BytesUtil {

    public static byte[] intToBytes(int i){
        ByteBuffer bb = ByteBuffer.allocate(4);
        bb.putInt(i);
        return bb.array();
    }

    public static int bytesToInt(byte[] intBytes){
        ByteBuffer bb = ByteBuffer.wrap(intBytes);
        return bb.getInt();
    }

}
