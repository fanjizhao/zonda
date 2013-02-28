bin_PROGRAMS = seq_service
seq_service_SOURCES = main.cpp
seq_service_LDFLAGS = -lpthread -lrt -llog4cpp -lzmq -lthrift -lzookeeper_mt
seq_service_CPPFLAGS= -I../../ -I../../common -I/usr/local/include/thrift -I/usr/local/include/c-client-src/
#seq_service_LDADD = ../../def/libdef.a ../../common/libcommon.la
seq_service_LDADD = ../../common/libcommon.a ../../def/libdef.a 
