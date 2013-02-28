#!/bin/bash

CLASSPATH="target/test-classes:target/classes"
# mvn dependency:copy-dependencies -DoutputDirectory=lib -DincludeScope=compile
for jarfile in ./lib/*.jar
do
        CLASSPATH="${CLASSPATH}:$jarfile"
        #echo $jarfile
done
CLASSPATH=$CLASSPATH:./src/resources/log4j.properties

#echo ${CLASSPATH}
#export CLASSPATH
#LD_LIBRARY_PATH=/usr/local/lib /opt/jdk1.7.0_04/bin/java -cp ${CLASSPATH} com.alibaba.platform.zonda.samples.client.Client
LD_LIBRARY_PATH=/usr/local/lib java -cp ${CLASSPATH} com.alibaba.platform.zonda.samples.client.Client

