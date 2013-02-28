bin_PROGRAMS = route_admin
route_admin_SOURCES = route_admin.cpp BizHandler.cpp
route_admin_CXXFLAGS = -ggdb
route_admin_LDFLAGS = -lpthread -lrt -llog4cpp -lzmq -lthrift -lzookeeper_mt
route_admin_CPPFLAGS = -I../common -I/usr/local/include/thrift -I../ -I/usr/local/include/c-client-src/
route_admin_LDADD = ../common/libcommon.a 
