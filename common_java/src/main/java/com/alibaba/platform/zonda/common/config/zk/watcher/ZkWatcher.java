package com.alibaba.platform.zonda.common.config.zk.watcher;

import com.alibaba.platform.zonda.common.config.Configer;
import com.alibaba.platform.zonda.common.config.event.ConfigChangeEvent;
import com.alibaba.platform.zonda.common.config.zk.ZkConfiger;
import org.apache.zookeeper.WatchedEvent;
import org.apache.zookeeper.Watcher;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import com.alibaba.platform.zonda.common.seda.*;
import com.alibaba.platform.zonda.common.rpc.Constants;
import com.alibaba.platform.zonda.logger.lib.AliLoggerFactory;

/**
 * ZooKeeper watcher implementation
 * User: darwin.yangq
 * Date: 12-1-16
 * Time: 7:46 pm
 */
public class ZkWatcher implements Watcher {

    protected Logger logger = AliLoggerFactory.getLogger(Constants.MODULE_NAME, getClass());
    
    protected Configer configer;

    /**
     * key: string, zookeeper node path
     * value: collection, IEventSink collection which needs the change
     * of the path
     */
    private Map<String, HashSet<IEventSink>> sinks =
            new ConcurrentHashMap<String, HashSet<IEventSink>>();

    public ZkWatcher(Configer configer) {
        this.configer = configer;
    }
    
    /**
     * all the asynchronous response messages from zookeeper will be thrown here
     * @param event
     */
    public void process(WatchedEvent event) {
        
    }
    
    protected Collection<IEventSink> getQueue(String path) {
        logger.debug("get queue with path: {}", path);
        ArrayList<IEventSink> list = new ArrayList<IEventSink>();
        HashSet<IEventSink> watchedQueues = this.sinks.get(path);
        if ( watchedQueues != null) {
            list.addAll(watchedQueues);
        }
        return list;
    }

    public void insertQueue(String path, IEventSink queue) {
        if (null == path || null == queue) {
            logger.error("path or queue is null, set watcher:{} failed", getClass());
        }

        logger.info("current watcher: {}", getClass().getName());
        logger.info("insert path: {}'s queue:{} for current watcher", path, queue.hashCode());
        HashSet<IEventSink> sinkSet = this.sinks.get(path);
        if (null == sinkSet) {
            logger.debug("no collection exists for path: {}", path);
            sinkSet = new HashSet<IEventSink>();
            this.sinks.put(path, sinkSet);
        }
        sinkSet.add(queue);
    }

    public void deleteQueue(String path, IEventSink queue) {
        if (null == path || null == queue) {
            logger.error("path or queue is null, set watcher:{} failed", getClass());
        }

        logger.info("current watcher: {}", getClass().getName());
        logger.info("delete path: {}'s queue:{} for watcher: {}", path, queue.hashCode());
        HashSet<IEventSink> sinkSet = this.sinks.get(path);
        if (null == sinkSet) {
            logger.info("no collection exists for path: {}", path);
            logger.info("no queue delete action");
        } else {
            sinkSet.remove(queue);
        }
    }

}
