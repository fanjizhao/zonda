package com.alibaba.platform.zonda.common.config.zk;

import com.alibaba.platform.zonda.common.config.*;
import com.alibaba.platform.zonda.common.config.event.ConfigChangeEvent;
import com.alibaba.platform.zonda.common.config.exception.*;
import com.alibaba.platform.zonda.common.config.zk.watcher.ExistsWatcher;
import com.alibaba.platform.zonda.common.config.zk.watcher.GetChildrenWatcher;
import com.alibaba.platform.zonda.common.config.zk.watcher.GetDataWatcher;
import com.alibaba.platform.zonda.common.config.zk.watcher.ZkWatcher;
import com.alibaba.platform.zonda.logger.lib.AliLoggerFactory;
import org.apache.zookeeper.*;
import org.apache.zookeeper.CreateMode;
import org.apache.zookeeper.data.Stat;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import com.alibaba.platform.zonda.common.seda.*;

import static org.apache.zookeeper.CreateMode.*;

/**
 * ZooKeeper configuration library implementation
 * User: darwin.yangq
 * Date: 12-1-16
 * Time: 7:26 pm
 */
public class ZkConfiger implements Configer {
    
    private Logger logger = AliLoggerFactory.getLogger(Constants.ZONDA_CONFIG_MODULE, getClass());

    // zookeeper client
    private ZooKeeper zooKeeper;

    // watcher for zookeeper client
    private ZkWatcher zkWatcher;

    // watcher for exists operation
    private ExistsWatcher existsWatcher;

    // watcher for getData operation
    private GetDataWatcher dataWatcher;

    // watcher for getChildren operation
    private GetChildrenWatcher childrenWatcher;

    private static Configer singleInstance = null;
    private static synchronized void createInstance()
    {
        if (singleInstance == null)
        {
            Configer obj = new ZkConfiger();
            singleInstance = obj;
        }
    }
    public static Configer getInstance()
    {
        if (singleInstance == null)
        {
            createInstance();
        }
        return singleInstance;
    }


    /**
     * initialize connection with specified scheme:auth information
     * @param metaInfo
     * @param authInfo
     * @throws InitConfigerFailedException
     */
    public synchronized void initialize(final MetaInfo metaInfo, final AuthInfo authInfo)
            throws InitConfigerFailedException {

        if (null == zooKeeper) {
            try {
                if (null == metaInfo) {
                    logger.error("no enough meta info provided, create zookeeper client failed");
                    return;
                } else {
                    logger.debug("connection string: {}", metaInfo.getConnectString());
                    logger.debug("session timeout: {}", metaInfo.getSessionTimeout());
                    this.zkWatcher = new ZkWatcher(this);
                    zooKeeper = new ZooKeeper(metaInfo.getConnectString(), metaInfo.getSessionTimeout(), zkWatcher);

                    // initialize four specific watchers
                    this.existsWatcher = new ExistsWatcher(this);
                    this.dataWatcher = new GetDataWatcher(this);
                    this.childrenWatcher = new GetChildrenWatcher(this);
                }
            } catch (IOException e) {
                logger.error("create zookeeper client failed, reason: {}", e);
                throw new InitConfigerFailedException(e.getMessage());
            }
        } else {
            logger.info("config library instance already exists");
        }
    }

    /**
     * close zookeeper config client connection
     * @throws ConfigUnintializedException
     * @throws CloseConfigerFailedException
     */
    public synchronized void close() throws ConfigUnintializedException, CloseConfigerFailedException {

        if (null != zooKeeper) {
            try {
                this.zooKeeper.close();
            } catch (InterruptedException e) {
                logger.error("close zookeeper client failed, reason: {}", e);
                throw new CloseConfigerFailedException(e.getMessage());
            }
        } else {
            logger.error("config library instance has not initialized yet");
            throw new ConfigUnintializedException();
        }
    }

    /**
     * create a node with the given path. The node data will be the given data
     * @param path
     * @param data
     * @param createMode
     * @throws ConfigUnintializedException
     * @throws CreateNodeFailedException
     */
    public String create(String path, byte[] data, com.alibaba.platform.zonda.common.config.CreateMode createMode)
            throws ConfigUnintializedException, CreateNodeFailedException {

        if (null != zooKeeper) {

            if (null == path) {
                logger.error("path is null, create failed");
                throw new CreateNodeFailedException("path is null");
            }

            logger.debug("create path: {}", path);
            logger.debug("data length: {}", (data == null ? 0 : data.length));

            try {
                return this.zooKeeper.create(path, data, ZooDefs.Ids.OPEN_ACL_UNSAFE, CreateMode.fromFlag(createMode.toFlag()));
            } catch (Exception e) {
                logger.error("create node failed, reason: {}", e);
                throw new CreateNodeFailedException(e.getMessage());
            }
        } else {
            logger.error("config library instance has not initialized yet");
            throw new ConfigUnintializedException();
        }
    }

    /**
     * create a node with the given path. The node data will be the given data
     * @param path
     * @param data
     * @throws ConfigUnintializedException
     * @throws CreateNodeFailedException
     */
    public String create(String path, byte[] data)  throws ConfigUnintializedException, CreateNodeFailedException {
        return this.create(path, data, com.alibaba.platform.zonda.common.config.CreateMode.PERSISTENT);
    }

    /**
     * return true of the node of the given path. Return false if no such a node exists
     * @param path
     * @throws ConfigUnintializedException
     * @throws QueryNodeFailedException
     */
    public boolean exists(String path) throws ConfigUnintializedException, QueryNodeFailedException {

        if (null != zooKeeper) {

            if (null == path) {
                logger.error("path is null, exists failed");
                throw new QueryNodeFailedException("path is null");
            }

            logger.debug("exists path: {}", path);
            Stat stat = null;

            try {
                stat = this.zooKeeper.exists(path, this.existsWatcher);
            } catch (Exception e) {
                logger.error("exists path: {} failed, reason: {}", path, e);
                throw new QueryNodeFailedException(e.getMessage());
            }

            logger.debug("exists result: {}", stat);
            return stat != null;

        } else {
            logger.error("config library instance has not initialized yet");
            throw new ConfigUnintializedException();
        }
    }

    /**
     * return true of the node of the given path. Return false if no such a node exists
     * @param path
     * @param sink, asynchronous response message queue
     * @param watch, register or un-register the path watcher
     * @return
     * @throws ConfigUnintializedException
     * @throws QueryNodeFailedException
     */
    public boolean exists(String path, IEventSink sink, boolean watch) throws ConfigUnintializedException,
            QueryNodeFailedException {

        if (null != zooKeeper) {

            if (null == path || null == sink) {
                logger.error("path or sink is null, return false");
                throw new QueryNodeFailedException("path or sink is null");
            }

            logger.debug("exists path: {}", path);
            logger.debug("exists watch: {}", watch);
            logger.debug("exists sink: {}", sink.hashCode());
            logger.debug("exists watcher: {}", this.existsWatcher);
            Stat stat = null;

            try {
                stat = this.zooKeeper.exists(path, this.existsWatcher);
                if (watch) {
                    this.existsWatcher.insertQueue(path, sink);
                } else {
                    this.existsWatcher.deleteQueue(path, sink);
                }
            } catch (Exception e) {
                logger.error("exists path: {} failed, reason: {}", path, e);
                throw new QueryNodeFailedException(e.getMessage());
            }

            logger.debug("exists result: {}", stat);
            return stat != null;

        } else {
            logger.error("config library instance has not initialized yet");
            throw new ConfigUnintializedException();
        }
    }

    /**
     * delete the node with the given path
     * @param path
     * @throws ConfigUnintializedException
     * @throws DeleteNodeFailedException
     */
    public void delete(String path) throws ConfigUnintializedException, DeleteNodeFailedException {

        if (null != zooKeeper) {
            
            if (null == path) {
                logger.error("path is null, delete failed");
                throw new DeleteNodeFailedException("path is null");
            }

            try {
                logger.debug("delete path: {}", path);
                this.zooKeeper.delete(path, -1);
            } catch (Exception e) {
                logger.error("delete path: {} failed, reason: {}", path, e);
                throw new DeleteNodeFailedException(e.getMessage());
            }
        } else {
            logger.error("config library instance has not initialized yet");
            throw new ConfigUnintializedException();
        }
    }

    /**
     * return the data of the node of the given path
     * @param path
     * @return data
     * @throws ConfigUnintializedException
     * @throws QueryValueFailedException
     */
    public byte[] getValue(String path) throws ConfigUnintializedException, QueryValueFailedException {

        byte[] data = new byte[0];
        if (null != zooKeeper) {

            if (null == path) {
                logger.error("path is null, get value failed");
                throw new QueryValueFailedException("path is null");
            }

            try {
                logger.debug("get value path: {}", path);
                data = this.zooKeeper.getData(path, false, null);
            } catch (Exception e) {
                logger.error("delete path: {} failed, reason: {}", path, e);
                throw new QueryValueFailedException(e.getMessage());
            }
        } else {
            logger.error("config library instance has not initialized yet");
            throw new ConfigUnintializedException();
        }
        return data;
    }

    /**
     * return the data of the node of the given path
     * @param path
     * @param sink, asynchronous response message queue
     * @param watch, register or un-register the path watcher
     * @return data
     * @throws ConfigUnintializedException
     * @throws QueryValueFailedException
     */
    public byte[] getValue(String path, IEventSink sink, boolean watch) throws ConfigUnintializedException,
            QueryValueFailedException {

        byte[] data = new byte[0];
        if (null != zooKeeper) {

            if (null == path || null == sink) {
                logger.error("path or sink is null, return false");
                throw new QueryValueFailedException("path or sink is null");
            }

            logger.debug("get value path: {}", path);
            logger.debug("get value watch: {}", watch);
            logger.debug("get value sink: {}", sink.hashCode());

            try {
                if (watch) {
                    data = this.zooKeeper.getData(path, this.dataWatcher, null);
                    this.dataWatcher.insertQueue(path, sink);
                } else {
                    data = this.zooKeeper.getData(path, false, null);
                    this.dataWatcher.deleteQueue(path, sink);
                }
            } catch (Exception e) {
                logger.error("delete path: {} failed, reason: {}", path, e);
                throw new QueryValueFailedException(e.getMessage());
            }
        } else {
            logger.error("config library instance has not initialized yet");
            throw new ConfigUnintializedException();
        }
        
        logger.debug("get value data size: {}", (data == null ? 0 : data.length) );
        return data;
    }

    /**
     * set the data for the node of the given path if such a node exists
     * @param path
     * @param data
     * @throws ConfigUnintializedException
     * @throws SetValueFailedException
     */
    public void setValue(String path, byte[] data) throws ConfigUnintializedException, SetValueFailedException {

        if (null != this.zooKeeper) {

            if (null == path) {
                logger.debug("path is null, set value failed");
                throw new SetValueFailedException("path is null");

            } else {
                logger.debug("set value path: {}", path);
                logger.debug("set value data length: {}", data == null ? 0 : data.length);

                try {
                    this.zooKeeper.setData(path, data, -1);
                } catch (Exception e) {
                    logger.error("set value for path: {} failed, reason: {}", path, e);
                    throw new SetValueFailedException(e.getMessage());
                }
            }
        } else {
            logger.error("config library instance has not initialized yet");
            throw new ConfigUnintializedException();
        }
    }

    /**
     * return the list of the children of the node of the given path
     * @param path
     * @return children collection
     * @throws ConfigUnintializedException
     * @throws QueryChildrenFailedException
     */
    public List<String> getChildren(String path) throws ConfigUnintializedException, QueryChildrenFailedException {

        List<String> children = new ArrayList<String>(0);
        if (null != this.zooKeeper) {

            if (null == path) {
                logger.debug("path is null, get children failed");
                throw new QueryChildrenFailedException("path is null");
            } else {
                try {
                    logger.debug("set value path: {}", path);
                    children = this.zooKeeper.getChildren(path, false);
                } catch (Exception e) {
                    logger.error("get children for path: {} failed, reason: {}", path, e);
                    throw new QueryChildrenFailedException(e.getMessage());
                }
            }
        } else {
            logger.error("config library instance has not initialized yet");
            throw new ConfigUnintializedException();
        }
        return children;
    }

    /**
     * return the list of the children of the node of the given path
     * @param path
     * @param sink, asynchronous response message queue
     * @param watch, register or un-register the path watcher
     * @return children collection
     * @throws ConfigUnintializedException
     * @throws QueryChildrenFailedException
     */
    public List<String> getChildren(String path, IEventSink sink, boolean watch) throws ConfigUnintializedException,
            QueryChildrenFailedException {

        List<String> children = new ArrayList<String>(0);
        if (null != this.zooKeeper) {

            if (null == path || null == sink) {
                logger.error("path or sink is null, get children failed");
                throw new QueryChildrenFailedException("path or sink is null");

            } else {
                logger.debug("get children path: {}", path);
                logger.debug("get children sink: {}", sink.hashCode());
                logger.debug("get children watch: {}", watch);

                try {
                    if (watch) {
                        children = this.zooKeeper.getChildren(path, this.childrenWatcher);
                        this.childrenWatcher.insertQueue(path, sink);
                    } else {
                        children = this.zooKeeper.getChildren(path, false);
                        this.childrenWatcher.deleteQueue(path, sink);
                    }
                } catch (Exception e) {
                    logger.error("get children for path: {} failed, reason: {}", path, e);
                    throw new QueryChildrenFailedException(e.getMessage());
                }
            }
        } else {
            logger.error("config library instance has not initialized yet");
            throw new ConfigUnintializedException();
        }
        return children;
    }

}
