#AM_CPPFLAGS = -I../
#LDADD = ../libcommon.a
#AM_LDFLAGS = -lpthread -lrt
bin_PROGRAMS = singleton_test thread_test stage_test timer_test zmq_trans_test \
	byte_order_test log_lib_test TestSplitStr md5_test \
	service_grouping_table_test
singleton_test_SOURCES = singleton_test.cpp
thread_test_SOURCES = thread_test.cpp
stage_test_SOURCES = stage_test.cpp
timer_test_SOURCES = timer_test.cpp

singleton_test_LDFLAGS = -lpthread -lrt -llog4cpp -lzmq
singleton_test_CPPFLAGS= -I../
singleton_test_LDADD = ../libcommon.a 
singleton_test_CXXFLAGS = -g

thread_test_LDFLAGS = -lpthread -lrt -llog4cpp -lzmq
thread_test_CPPFLAGS= -I../
thread_test_LDADD = ../libcommon.a 
thread_test_CXXFLAGS = -g

stage_test_LDFLAGS = -lpthread -lrt -llog4cpp -lzmq
stage_test_CPPFLAGS= -I$(top_srcdir)/common
stage_test_CXXFLAGS= -ggdb
stage_test_LDADD = ../libcommon.a 

timer_test_LDFLAGS = -lpthread -lrt -llog4cpp -lzmq
timer_test_CPPFLAGS= -I../
timer_test_LDADD = ../libcommon.a 
timer_test_CXXFLAGS = -g

zmq_trans_test_SOURCES = zmq_trans_test.cpp
zmq_trans_test_LDFLAGS = -lpthread -lrt -lzmq -llog4cpp
zmq_trans_test_CPPFLAGS= -I../
zmq_trans_test_LDADD = ../libcommon.a 
zmq_trans_test_CXXFLAGS = -g

byte_order_test_SOURCES = byte_order_test.cpp
byte_order_test_LDFLAGS = -lpthread -lrt -lzmq -llog4cpp
byte_order_test_CPPFLAGS= -I../
byte_order_test_LDADD = ../libcommon.a 
byte_order_test_CXXFLAGS = -g


log_lib_test_SOURCES = log_lib_test.cpp
log_lib_test_LDFLAGS = -lpthread -lrt -lzmq -llog4cpp
log_lib_test_CPPFLAGS= -I../
log_lib_test_LDADD = ../libcommon.a
log_lib_test_CXXFLAGS = -g

#TestConfig_SOURCES = TestConfig.cpp
#TestConfig_LDFLAGS = -lpthread -lrt -lzmq -llog4cpp -lzookeeper_mt
#TestConfig_CPPFLAGS= -I../ -I/usr/local/include/c-client-src
#TestConfig_LDADD = ../libcommon.a
#TestConfig_CXXFLAGS = -g -DTHREADED

TestSplitStr_SOURCES = TestSplitStr.cpp
TestSplitStr_CPPFLAGS= -I../
TestSplitStr_LDADD = ../libcommon.a
TestSplitStr_CXXFLAGS = -g

#TestConfigTransfer_SOURCES = TestConfigTransfer.cpp
#TestConfigTransfer_LDFLAGS = -lpthread -llog4cpp
#TestConfigTransfer_CPPFLAGS= -I../
#TestConfigTransfer_LDADD = ../libcommon.a
#TestConfigTransfer_CXXFLAGS = -g

md5_test_SOURCES = md5_test.cpp
md5_test_CPPFLAGS= -I../
md5_test_LDADD = ../libcommon.a

service_grouping_table_test_SOURCES = service_grouping_table_test.cpp
service_grouping_table_test_LDFLAGS = -lpthread -llog4cpp
service_grouping_table_test_CPPFLAGS = -I../
service_grouping_table_test_LDADD = ../libcommon.a
service_grouping_table_test_CXXFLAGS = -g 
