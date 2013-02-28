package com.alibaba.zonda.logger.watcher;

import java.util.List;

import junit.framework.Assert;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.alibaba.zonda.logger.watcher.ConsoleAggregator;
import com.alibaba.zonda.logger.watcher.config.TimeTunnelConfiguration;
import com.alibaba.zonda.logger.watcher.filter.Filter;

public class ConsoleAggregatorTest {
	
	private ConsoleAggregator consoleAggregator;

	@Before
	public void setUp() throws Exception {
		Filter filter = new Filter();
		//filter.setShowHostname(false);
		//filter.setShowLineNumber(false);
		//filter.setShowLogLevel(false);
		//filter.setHostnames("inc-platform-dev-153-132.hst.bjc.kfc.alidc.net");
		//filter.setModules("test_modle");
		//filter.setTxIds("123");
		//filter.setLevels("I,D");
        consoleAggregator = new ConsoleAggregator(new TimeTunnelConfiguration("watcher.properties"), filter);
	}

	@After
	public void tearDown() throws Exception {
	
	}

	@Test
	public void testConsoleAggregator() {
		
	}

	@Test
	public void testSetConfiguration() {
		
	}

	@Test
	public void testSetFilter() {
		
	}
	
	//@Test
	public void testConvertString2LogSegments() {
		String str = "20111219 17:1:41.584833 T [inc-platform-dev-153-132.hst.bjc.kfc.alidc.net:test_module:category:Sampler.cpp:25:main:123] - trace something: 4";
		List<ConsoleAggregator.LogSegment> segments = consoleAggregator.convertString2LogSegments(str);
		
		Assert.assertEquals(1, segments.size());
		ConsoleAggregator.LogSegment segment = segments.get(0);
		Assert.assertEquals("20111219 17:1:41.584833", segment.timestamp);
		Assert.assertEquals("T", segment.level);
		Assert.assertEquals("inc-platform-dev-153-132.hst.bjc.kfc.alidc.net", segment.hostname);
		Assert.assertEquals("test_module", segment.module);
		Assert.assertEquals("category", segment.category);
		Assert.assertEquals("Sampler.cpp", segment.clazz);
		Assert.assertEquals("25", segment.lineNumber);
		Assert.assertEquals("main", segment.method);
		Assert.assertEquals("123", segment.txId);
		Assert.assertEquals("trace something: 4", segment.msg);
	}

	public static void main(String[] args) {
        Filter filter = new Filter();
        ConsoleAggregator consoleAggregator = new ConsoleAggregator(new TimeTunnelConfiguration("watcher.properties"), filter);
        consoleAggregator.execute();
	}

}
