/**
 * Project: zonda.logger.server
 *
 * File Created at 12-2-3
 * OutputStreamManager: OutputStreamManager.java 12-2-3 darwin $
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

import com.alibaba.zonda.logger.server.config.Configure;
import com.alibaba.zonda.logger.server.util.BytesUtil;
import com.alibaba.zonda.logger.server.util.DateUtil;
import com.alibaba.zonda.logger.server.util.FileUtil;
import org.apache.commons.io.IOUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.util.Map;
import java.util.UUID;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.locks.ReentrantReadWriteLock;

/**
 * User: darwin
 * Date: 12-2-3
 * Time: 11:41 am
 */
public class OutputStreamManager {

    private static final Configure configure = Configure.instance();
    private static final Logger logger = LoggerFactory.getLogger(OutputStreamManager.class);
    private static OutputStreamManager instance = new OutputStreamManager(configure.getLog_base_dir());

    private final ReentrantReadWriteLock lock;
    private Map<String, LogOutputStream> osMap;
    private String baseDir;
    private OutputStreamNameGenerator filenameGenerator = new DefaultOutputStreamNameGenerator();

    public static OutputStreamManager getInstance() {
        return instance;
    }

    private void setFilenameGenerator(OutputStreamNameGenerator filenameGenerator) {
        this.filenameGenerator = filenameGenerator;
    }

    private OutputStreamManager(String baseDir) {
        this.lock = new ReentrantReadWriteLock();
        this.baseDir = baseDir;
        this.osMap = new ConcurrentHashMap<String, LogOutputStream>(5);
    }

    public void switchAllOutputStreams() throws IOException {
        lock.writeLock().lock();
        try {
            for (String tag : osMap.keySet()) {
                LogOutputStream os = osMap.get(tag);
                if (null != os && null != os.getStream()) {
                    if (os.getBytesWritten() == 0) {
                        deleteOutputStream(os);
                    } else {
                        closeOutputStream(os);
                    }
                }
                os = newOutputStream(tag);
                osMap.put(tag, os);
            }
        } finally {
            lock.writeLock().unlock();
        }
    }

    public void closeAllOutputStreams() throws IOException {
        lock.writeLock().lock();
        try {
            for (String tag : osMap.keySet()) {
                LogOutputStream os = osMap.get(tag);
                if (null != os && null != os.getStream()) {
                    if (os.getBytesWritten() == 0) {
                        deleteOutputStream(os);
                    } else {
                        closeOutputStream(os);
                    }
                }
            }
        } finally {
            lock.writeLock().unlock();
        }
    }

    private void deleteOutputStream(LogOutputStream os) {
        try {
            os.getStream().close();
            new File(os.getFilePath()).delete();
        } catch (IOException e) {
            logger.error("close file failed for delete", e);
        }
    }

    private LogOutputStream newOutputStream(String tag) throws IOException {
        LogOutputStream os = new LogOutputStream();
        FileUtil.ensurePathExists(baseDir + "/" + tag);
        os.setFilePath(filenameGenerator.generateFileName(baseDir, tag));
        logger.debug("create new file: {}", os.getFilePath());
        os.setStream(new FileOutputStream(os.getFilePath(), true));
        os.setBytesWritten(0);
        os.setMessagesWritten(0);
        return os;
    }

    private void closeOutputStream(LogOutputStream os) throws IOException {
        os.getStream().close();
        String newName = os.getFilePath().substring(0, os.getFilePath().length() - 4);
        new File(os.getFilePath()).renameTo(new File(newName));
    }

    private LogOutputStream getOutputStream(String tag) throws IOException {
        LogOutputStream os = osMap.get(tag);
        if (null != os && os.getBytesWritten() > configure.getMax_log_file_size()) {
            closeOutputStream(os);
            os = null;
        }
        if (null == os || null == os.getStream()) {
            os = newOutputStream(tag);
            osMap.put(tag, os);
        }
        return os;
    }

    public void writeWithLength(byte[] data, String tag) throws IOException {
        lock.readLock().lock();
        try {
            LogOutputStream os = getOutputStream(tag);
            ByteBuffer bf = ByteBuffer.allocate(4 + data.length);
            bf.put(BytesUtil.intToBytes(data.length));
            bf.put(data);
            bf.flip();
            IOUtils.write(bf.array(), os.getStream());
            os.addBytesWritten(4 + data.length);
            os.getStream().flush();
        } finally {
            lock.readLock().unlock();
        }
    }

    public void write(byte[] data, String tag) throws IOException {
        lock.readLock().lock();
        try {
            LogOutputStream os = getOutputStream(tag);
            IOUtils.write(data, os.getStream());
            os.addBytesWritten(data.length);
            os.getStream().flush();
        } finally {
            lock.readLock().unlock();
        }
    }

}
