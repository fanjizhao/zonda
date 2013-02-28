TARGET_CPP = gen-cpp/*.cpp gen-cpp/*.h

INC = -I../common -I/usr/local/include/thrift -I../inc
CXXFLAGS= -pg
all: libdef.a 

#${TARGET_CPP}: *.thrift
#	find ./ -name "*.thrift" -exec thrift --gen cpp {} \;
#	find ./ -name "*.thrift" -exec thrift -out "./java_def/src/java" --gen java {} \;

libdef.a: services.o

%.o: %.cpp
	g++ -I../common -o $@  -c $< $(CXXFLAGS) $(INC)
	
services.o: services.def ./thrift/*.thrift 
	./tools/service_gen.pl
	./tools/java_service_gen.pl
	find ./ -name "*.thrift" -exec thrift --gen cpp {} \;
	find ./ -name "*.thrift" -exec thrift -o "./java_def/src/java" --gen java:hashcode {} \;
	find ./ -name "*.cpp" -exec g++ -c $(INC) $(CXXFLAGS) {} \;
	ar -r libdef.a *.o 

clean:
	rm -rf gen-cpp
	rm -rf java_def/src/java/gen-java
	find java_def/src/java -name "*.java" -exec rm -rf {} \;
	rm -rf *.o
	rm -rf libdef.a
	rm -rf services.cpp class_msg_type-inc.h
