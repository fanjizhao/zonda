noinst_LIBRARIES = libcommon.a
#lib_LTLIBRARIES = libcommon.la
libcommon_a_SOURCES = thread/ThreadLock.cpp thread/Thread.cpp thread/ThreadMgr.cpp \
	seda/EventQueue.cpp thread/ThreadCond.cpp seda/StageThread.cpp \
	seda/StageMgr.cpp seda/EventDispatcher.cpp \
	util/string_ex.cpp util/TimerThread.cpp util/TimerService.cpp util/time_ex.cpp \
	util/TimerEvent.cpp \
	util/Configurator.cpp util/net_util.cpp util/GlobalTimer.cpp \
	util/md5_impl.c \
	log/Logger.cpp log/LoggerStream.cpp log/LoggerFactory.cpp \
	config/ZkConfigImpl.cpp config/Module.cpp \
	config/ConfigChangeEvent.cpp \
	rpc/RpcService.cpp rpc/ServiceStageMap.cpp rpc/RouteTable.cpp rpc/BufferTransport.cpp \
	rpc/RpcEvent.cpp  rpc/RpcEventHandlerBase.cpp rpc/RouteStrategy.cpp \
	rpc/ServiceGroupingTable.cpp rpc/GlobalServicesInfo.cpp rpc/ServiceInstanceRole.cpp \
	rpc/SyncRpcService.cpp rpc/CallOutRecord.cpp rpc/ZMQReceiver.cpp rpc/ZMQSender.cpp \
	rpc/TransOutEvent.cpp  rpc/ZMQTrans.cpp rpc/TransInEvent.cpp config/IConfig.cpp
	
	
libcommon_a_CPPFLAGS = -I../ -I/usr/local/include/thrift/ -I/usr/local/include/c-client-src/
libcommon_a_CXXFLAGS = -ggdb -Wall -pg
#libcommon_a_LDFLAGS = -pg
SUBDIRS= . test
