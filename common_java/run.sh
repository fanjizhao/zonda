#!/bin/bash

CLASSPATH="target/test-classes:target/classes"
# mvn dependency:copy-dependencies -DoutputDirectory=lib -DincludeScope=compile
for jarfile in ./lib/*.jar
do
        CLASSPATH="${CLASSPATH}:$jarfile"
        #echo $jarfile
done

#export CLASSPATH
#java -cp ${CLASSPATH} com.alibaba.platform.zonda.common.test.StageTest
#java -cp ${CLASSPATH} com.alibaba.platform.zonda.common.rpc.ServiceInstanceId
#java -cp ${CLASSPATH} com.alibaba.platform.zonda.common.rpc.RouteTable
#java -cp ${CLASSPATH} com.alibaba.platform.zonda.common.rpc.ServiceGroupingTable
#java -cp ${CLASSPATH} com.alibaba.platform.zonda.common.rpc.RouteStrategy
#java -cp ${CLASSPATH} com.alibaba.platform.zonda.common.util.NetUtil
#java -cp ${CLASSPATH} com.alibaba.platform.zonda.common.rpc.RpcHead
#java -cp ${CLASSPATH} com.alibaba.platform.zonda.common.rpc.ZMQSender
#java -cp ${CLASSPATH} com.alibaba.platform.zonda.common.rpc.CallOutRecord
java -cp ${CLASSPATH} com.alibaba.platform.zonda.common.util.GlobalTimer

