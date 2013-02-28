package com.alibaba.platform.zonda.logger.lib;

import junit.framework.TestCase;

import org.slf4j.Logger;

public class AliPatternLayoutTest extends TestCase {
	
	private Logger logger = AliLoggerFactory.getLogger("test", getClass());

	public void setUp() throws Exception {
		
	}

	public void tearDown() throws Exception {
	
	}
	
	public void testTrace() {
		logger.trace(TransactionTracer.TX + "test trace {} and {}", new Object[] {"12", "i", "you" });
	}		
	
	public void testDebug() {
		logger.debug("test debug {} and {}", "i", "you");
	}
	
	public void testInfo() {
		logger.info("test info {} and {}", "i", "you");
	}
	
	public void testWarn() {
		logger.warn("test warn {} and {}", "i", "you");
	}
	
	public void testError() {
		logger.error("test error {} and {}", "i", "you");
	}	
	
}
