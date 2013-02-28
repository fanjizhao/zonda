bin_PROGRAMS = client
client_CXXFLAGS = -ggdb
client_SOURCES = client.cpp
client_LDFLAGS = -lpthread -lrt -llog4cpp -lzmq -lthrift -lzookeeper_mt
client_CPPFLAGS= -I../../ -I../../common -I/usr/local/include/thrift -I/usr/local/include/c-client-src/
client_LDADD = ../../common/libcommon.a ../../def/libdef.a
#client_LDADD = ../../def/libdef.a ../../common/libcommon.la 
