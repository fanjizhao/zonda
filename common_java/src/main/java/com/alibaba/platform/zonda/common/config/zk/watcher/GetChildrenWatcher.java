package com.alibaba.platform.zonda.common.config.zk.watcher;

import com.alibaba.platform.zonda.common.config.Configer;
import com.alibaba.platform.zonda.common.config.event.ConfigChangeEvent;
import com.alibaba.platform.zonda.common.config.event.ChangeType;
import org.apache.zookeeper.WatchedEvent;

import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import com.alibaba.platform.zonda.common.seda.*;

/**
 * ZooKeeper children node watcher implementation
 * User: darwin.yangq
 * Date: 12-1-16
 * Time: 7:46 pm
 */
public class GetChildrenWatcher extends ZkWatcher {

    public GetChildrenWatcher(Configer configer) {
        super(configer);
    }

    /**
     * all the asynchronous node notification response messages from zookeeper will be thrown here
     * @param event
     */
    public void process(WatchedEvent event) {

        logger.debug("get children watcher path: {}", event.getPath());
        logger.debug("get children watcher event type: {}", event.getType().name());
        logger.debug("get children watcher state: {}", event.getState().name());

        Collection<IEventSink> sinks = getQueue(event.getPath());
        if (null != sinks) {
            Iterator<IEventSink> itr = sinks.iterator();
            while (itr.hasNext()) {

                IEventSink sink = itr.next();
                ConfigChangeEvent ce = new ConfigChangeEvent();
                ce.setPath(event.getPath());

                if (event.getType().equals(Event.EventType.NodeChildrenChanged)) {
                    try {
                        ce.setChangeType(ChangeType.NODE_CHILDREN_CHANGED);
                        List<String> children = this.configer.getChildren(event.getPath(), sink, true);
                        ce.setChildren(children);
                        sink.enqueue(ce);
                        logger.info("send children changed event to queue: {}", sink.hashCode());

                    } catch (Exception e) {
                        logger.error("process children changed event failed, reason: {}", e.getMessage());
                    }
                }
            }
        } else {
            logger.info("no queue collection found for path: {}", event.getPath());
        }

    }

}
