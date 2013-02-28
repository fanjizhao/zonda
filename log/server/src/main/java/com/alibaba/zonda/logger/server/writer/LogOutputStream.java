/**
 * Project: zonda.logger.server
 *
 * File Created at 12-2-3
 * : LogOutputStream.java 12-2-3 darwin $
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

import java.io.OutputStream;

/**
 * User: darwin
 * Date: 12-2-3
 * Time: 下午2:01
 */
public class LogOutputStream {

    private String filePath;
    private OutputStream stream;
    private long bytesWritten;
    private long messagesWritten;
    
    public LogOutputStream() {
        
    }
    
    public LogOutputStream(String filePath, OutputStream stream) {
        this.filePath = filePath;
        this.stream = stream;
    }

    public String getFilePath() {
        return filePath;
    }

    public void setFilePath(String filePath) {
        this.filePath = filePath;
    }

    public OutputStream getStream() {
        return stream;
    }

    public void setStream(OutputStream stream) {
        this.stream = stream;
    }

    public long getBytesWritten() {
        return bytesWritten;
    }

    public void setBytesWritten(long bytesWritten) {
        this.bytesWritten = bytesWritten;
    }

    public void addBytesWritten(long bytesWritten) {
        this.bytesWritten += bytesWritten;
    }

    public long getMessagesWritten() {
        return messagesWritten;
    }

    public void setMessagesWritten(long messagesWritten) {
        this.messagesWritten = messagesWritten;
    }

    public void addMessagesWritten(long messagesWritten) {
        this.messagesWritten += messagesWritten;
    }
}
