/**
 * Project: zonda.logger.server
 *
 * File Created at 12-2-3
 * FileUtil: FileUtil.java 12-2-3 darwin $
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

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.apache.commons.io.DirectoryWalker;

import java.io.File;
import java.io.IOException;
import java.util.*;

/**
 * User: darwin
 * Date: 12-2-3
 * Time: 11:27 am
 */
public class FileUtil {

    private static final Logger logger = LoggerFactory.getLogger(FileUtil.class);

    public static List<File> listDirectory(String path, String filterRegex) {
        try {
            return new DirectoryListener().list(new File(path),filterRegex);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public static List<File> listDirectory(String path, String dateStr, String filterRegex) {
        logger.debug("list dir: " + path + " and dateStr: " + dateStr);
        try {
            return new DirectoryListener(dateStr).list(new File(path), filterRegex);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public static String ensurePathExists(String path) {
        File f = new File(path);
        if (!f.exists())
            f.mkdirs();
        return path;
    }

    public static boolean rename(File f1, File f2) {
        ensurePathExists(f2.getParent());
        if (f2.exists())
            f2.delete();
        return f1.renameTo(f2);
    }

}
