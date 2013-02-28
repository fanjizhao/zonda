#!/bin/bash

CLASSPATH="target/test-classes:target/classes"
# mvn dependency:copy-dependencies -DoutputDirectory=lib -DincludeScope=compile
for jarfile in ./lib/*.jar
do
        CLASSPATH="${CLASSPATH}:$jarfile"
        #echo $jarfile
done

#export CLASSPATH
#LD_LIBRARY_PATH=/usr/local/lib /opt/jdk1.7.0_04/bin/java -cp ${CLASSPATH} com.alibaba.platform.zonda.samples.echo_service.EchoService
LD_LIBRARY_PATH=/usr/local/lib java -cp ${CLASSPATH} com.alibaba.platform.zonda.samples.seq_service.SeqService

