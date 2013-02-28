/**
 * Project: zonda.logger.server
 *
 * File Created at 12-2-5
 * : DirectoryListener.java 12-2-5 darwin $
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

import org.apache.commons.io.DirectoryWalker;

import java.io.File;
import java.io.IOException;
import java.util.*;

/**
 * User: darwin
 * Date: 12-2-5
 * Time: 1:19 pm
 */
public class DirectoryListener extends DirectoryWalker {

    private String dateStr;

    public DirectoryListener(String dateStr) {
        super();
        this.dateStr = dateStr;
    }

    public DirectoryListener() {
        super();
        this.dateStr = null;
    }

    public List<File> list(File startDirectory, String filterRegex) throws IOException {
        List<File> dirList = new ArrayList<File>();
        walk(startDirectory, dirList);
        Collections.sort(dirList, new Comparator<File>() {
            public int compare(File f1, File f2) {
                return (Long.valueOf(f1.lastModified())).compareTo(Long.valueOf(f2.lastModified()));
            }
        });
        List<File> filteredDirList = new ArrayList<File>();
        for (File f : dirList) {
            if (f.getName().matches(filterRegex)) {
                filteredDirList.add(f);
            }
        }
        return filteredDirList;
    }

    protected void handleFile(File file, int depth, Collection dirList) {
        if (dateStr == null) {
            if (!file.getName().endsWith(".tmp")) {
                dirList.add(file);
            }
        } else {
            if (!file.getName().endsWith(".tmp")
                    && file.getName().compareToIgnoreCase(dateStr) <= 0
                    || file.getName().startsWith(dateStr)) {
                dirList.add(file);
            }
        }
    }

}
