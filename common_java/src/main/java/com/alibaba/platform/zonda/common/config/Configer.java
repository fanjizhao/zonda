package com.alibaba.platform.zonda.common.config;

import com.alibaba.platform.zonda.common.config.event.ConfigChangeEvent;
import com.alibaba.platform.zonda.common.config.exception.*;

import java.util.List;
import com.alibaba.platform.zonda.common.seda.*;

/**
 * Configuration library
 * User: darwin.yangq
 * Date: 12-1-16
 * Time: 5:33 pm
 */
public interface Configer {
    
    public static final String ZONDA_CONF_PATH = "/zonda/conf";
    public static final String ZONDA_ROUTE_ADMIN_PATH = "/zonda/route_admin";
    public static final String ZONDA_ROUTE_TABLE_PATH = "/zonda/route_table";
    public static final String ZONDA_ROUTE_STRATEGY_PATH = "/zonda/route_strategy";
    public static final String ZONDA_RUNTIME_PATH = "/zonda/runtime";
    public static final String ZONDA_SERVICE_GROUPING_PATH = "/zonda/service_grouping";     
    
    /**
     * initialize connection with specified scheme:auth information
     * @param metaInfo
     * @param authInfo
     * @throws InitConfigerFailedException
     */
    public void initialize(final MetaInfo metaInfo, final AuthInfo authInfo) throws InitConfigerFailedException;

    /**
     * close config client connection
     * @throws ConfigUnintializedException
     * @throws CloseConfigerFailedException
     */
    public void close() throws ConfigUnintializedException, CloseConfigerFailedException;

    /**
     * create a node with the given path. The node data will be the given data
     * @param path
     * @param data
     * @throws ConfigUnintializedException
     * @throws CreateNodeFailedException
     */
    public String create(String path, byte[] data) throws ConfigUnintializedException, CreateNodeFailedException;

    /**
     * create a node with the given path. The node data will be the given data
     * @param path
     * @param data
     * @param createMode
     * @throws ConfigUnintializedException
     * @throws CreateNodeFailedException
     */
    public String create(String path, byte[] data, CreateMode createMode)
            throws ConfigUnintializedException, CreateNodeFailedException;

    /**
     * return true of the node of the given path. Return false if no such a node exists
     * @param path
     * @throws ConfigUnintializedException
     * @throws QueryNodeFailedException
     */
    public boolean exists(String path) throws ConfigUnintializedException, QueryNodeFailedException;

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
            QueryNodeFailedException;

    /**
     * delete the node with the given path
     * @param path
     * @throws ConfigUnintializedException
     * @throws DeleteNodeFailedException
     */
    public void delete(String path) throws ConfigUnintializedException, DeleteNodeFailedException;

    /**
     * return the data of the node of the given path
     * @param path
     * @return data
     * @throws ConfigUnintializedException
     * @throws QueryValueFailedException
     */
    public byte[] getValue(String path) throws ConfigUnintializedException, QueryValueFailedException;

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
            QueryValueFailedException;

    /**
     * set the data for the node of the given path if such a node exists
     * @param path
     * @param data
     * @throws ConfigUnintializedException
     * @throws SetValueFailedException
     */
    public void setValue(String path, byte[] data) throws ConfigUnintializedException, SetValueFailedException;

    /**
     * return the list of the children of the node of the given path
     * @param path
     * @return children collection
     * @throws ConfigUnintializedException
     * @throws QueryChildrenFailedException
     */
    public List<String> getChildren(String path) throws ConfigUnintializedException, QueryChildrenFailedException;

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
            QueryChildrenFailedException;

}
