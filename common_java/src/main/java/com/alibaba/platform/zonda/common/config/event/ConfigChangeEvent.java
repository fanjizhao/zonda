package com.alibaba.platform.zonda.common.config.event;

import java.util.ArrayList;
import java.util.List;
import com.alibaba.platform.zonda.common.seda.*;

/**
 * Config client event definition
 * User: darwin.yangq
 * Date: 12-1-17
 * Time: 5:11 pm
 */
public class ConfigChangeEvent implements IEvent {
    
    private ChangeType changeType;
    
    private String path;

    private byte[] data;

    private List<String> children = new ArrayList<String>(0);

    public int getType() {
        return EventType.CONFIG_CHANGE_EVENT;
    }
    
    public ChangeType getChangeType() {
        return changeType;
    }

    public void setChangeType(ChangeType type) {
        this.changeType = type;
    }

    public String getPath() {
        return path;
    }

    public void setPath(String path) {
        this.path = path;
    }

    public byte[] getData() {
        return data;
    }

    public void setData(byte[] data) {
        this.data = data;
    }

    public List<String> getChildren() {
        return children;
    }

    public void setChildren(List<String> children) {
        this.children = children;
    }
}
