bin_PROGRAMS = echo_service
echo_service_SOURCES = main.cpp
echo_service_LDFLAGS = -lpthread -lrt -llog4cpp -lzmq -lthrift -pg -lzookeeper_mt
echo_service_CPPFLAGS= -I../../ -I../../common -I/usr/local/include/thrift -I//usr/local/include/c-client-src/
echo_service_LDADD = ../../common/libcommon.a ../../def/libdef.a
#echo_service_LDADD = ../../def/libdef.a ../../common/libcommon.la 
echo_service_CXXFLAGS = -pg
