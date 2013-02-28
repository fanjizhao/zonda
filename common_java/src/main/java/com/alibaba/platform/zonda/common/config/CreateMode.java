package com.alibaba.platform.zonda.common.config;

import com.alibaba.platform.zonda.logger.lib.AliLoggerFactory;
import org.apache.log4j.Logger;

/**
 * User: darwin
 * Date: 12-1-29
 * Time: 5:04 pm
 */
public enum CreateMode {

    /**
     * The znode will not be automatically deleted upon client's disconnect.
     */
    PERSISTENT (0, false, false),
    /**
     * The znode will not be automatically deleted upon client's disconnect,
     * and its name will be appended with a monotonically increasing number.
     */
    PERSISTENT_SEQUENTIAL (2, false, true),
    /**
     * The znode will be deleted upon the client's disconnect.
     */
    EPHEMERAL (1, true, false),
    /**
     * The znode will be deleted upon the client's disconnect, and its name
     * will be appended with a monotonically increasing number.
     */
    EPHEMERAL_SEQUENTIAL (3, true, true);

    private static org.slf4j.Logger logger = AliLoggerFactory.getLogger(Constants.ZONDA_CONFIG_MODULE, CreateMode.class);

    private boolean ephemeral;
    private boolean sequential;
    private int flag;

    CreateMode(int flag, boolean ephemeral, boolean sequential) {
        this.flag = flag;
        this.ephemeral = ephemeral;
        this.sequential = sequential;
    }

    public boolean isEphemeral() {
        return ephemeral;
    }

    public boolean isSequential() {
        return sequential;
    }

    public int toFlag() {
        return flag;
    }

    /**
     * Map an integer value to a CreateMode value
     */
    static public CreateMode fromFlag(int flag) throws Exception {
        switch(flag) {
            case 0: return CreateMode.PERSISTENT;

            case 1: return CreateMode.EPHEMERAL;

            case 2: return CreateMode.PERSISTENT_SEQUENTIAL;

            case 3: return CreateMode.EPHEMERAL_SEQUENTIAL ;

            default:
                logger.error("Received an invalid flag value to convert to a CreateMode");
                throw new Exception();
        }
    }

}


