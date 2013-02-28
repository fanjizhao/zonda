#!/bin/bash

#BASEDIR=`dirname $0`
cd ..
CLASSPATH=.

LIBPATH="lib"

export LIBPATH

for f in `find $LIBPATH -name '*.jar'`
  do
    CLASSPATH=$CLASSPATH:$f
  done

# ******************************************************************
# ** Set java runtime options                                     **
# ** Change 256m to higher values in case you run out of memory.  **
# ******************************************************************

#export DEBUG=-Xdebug -Xrunjdwp:transport=dt_socket,address=3005,server=y,suspend=n
OPT="-Xmx1024m -cp $CLASSPATH"

# ***************
# ** Run...    **
# ***************


java -jar ../lib/zonda-logger-server-0.1.0.jar monitor