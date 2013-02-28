package com.alibaba.platform.zonda.common.config.zk.watcher;

import com.alibaba.platform.zonda.common.config.Configer;
import com.alibaba.platform.zonda.common.config.event.ConfigChangeEvent;
import com.alibaba.platform.zonda.common.config.event.ChangeType;
import com.alibaba.platform.zonda.common.config.exception.ConfigUnintializedException;
import com.alibaba.platform.zonda.common.config.exception.QueryValueFailedException;
import org.apache.zookeeper.WatchedEvent;

import java.util.Collection;
import java.util.Iterator;
import com.alibaba.platform.zonda.common.seda.*;

/**
 * ZooKeeper node exist watcher implementation
 * User: darwin.yangq
 * Date: 12-1-17
 * Time: 11:09 am
 */
public class ExistsWatcher extends ZkWatcher {

    public ExistsWatcher(Configer configer) {
        super(configer);
    }

    /**
     * all the exists call asynchronous response messages from zookeeper will be thrown here
     * @param event
     */
    public void process(WatchedEvent event) {

        logger.debug("exists watcher path: {}", event.getPath());
        logger.debug("exists watcher event type: {}", event.getType().name());
        logger.debug("exists watcher state: {}", event.getState().name());

        Collection<IEventSink> sinks = getQueue(event.getPath());
        if (null != sinks) {
            Iterator<IEventSink> itr = sinks.iterator();
            while (itr.hasNext()) {

                IEventSink sink = itr.next();
                ConfigChangeEvent ce = new ConfigChangeEvent();
                ce.setPath(event.getPath());

                if (event.getType().equals(Event.EventType.NodeCreated)) {
                    try {
                        ce.setChangeType(ChangeType.NODE_CREATED);
                        byte[] data = this.configer.getValue(event.getPath(), sink, true);
                        ce.setData(data);
                        sink.enqueue(ce);
                        logger.info("send node create event to queue: {}", sink.hashCode());
                        
                    } catch (Exception e) {
                        logger.error("process node created event failed, reason: {}", e.getMessage());
                    }
                } else if(event.getType().equals(Event.EventType.NodeDeleted)) {

                    ce.setChangeType(ChangeType.NODE_DELETED);
                    sink.enqueue(ce);
                    logger.info("send node delete event to queue: {}", sink.hashCode());

                } else if(event.getType().equals(Event.EventType.NodeDataChanged)) {
                    try {
                        ce.setChangeType(ChangeType.DATA_CHANGED);
                        byte[] data = this.configer.getValue(event.getPath(), sink, true);
                        ce.setData(data);
                        sink.enqueue(ce);
                        logger.info("send data changed event to queue: {}", sink.hashCode());

                    } catch (Exception e) {
                        logger.error("process node created event failed, reason: {}", e.getMessage());
                    }
                }
            }
        } else {
            logger.info("no queue collection found for path: {}", event.getPath());
        }
    }

}
