package com.alibaba.zonda.logger.server.writer;

/**
 * User: darwin
 * Date: 12-2-3
 * Time: 1:56 pm
 */
public interface OutputStreamNameGenerator {

    public String generateFileName(String baseDir, String tag);

}
