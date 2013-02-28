#!/bin/bash

usage () {
	echo ""
	echo "Usage: `basename $0` "
	exit 1
}

#############################################################
# check MySQL
#############################################################
check_mysql() {
	mysqlclient=`which mysql`
	[ -z "$mysqlclient" ] && echo "no mysql client found" && exit -1
}

#############################################################
# check thrift/zeromq
#############################################################
check_thrift() {
	TV=`thrift -version`
	[ $? -eq 0 ] && echo "no thrift installed" && exit -1
	echo $TV
	ZMQ=`find /usr/local/lib -name libzmq.so`
	[ -z "$ZMQ" ] && echo "no zmq lib found." && exit -1
	ZJMQ=`find /usr/local/lib -name libjzmq.so`
	[ -z "$ZJMQ" ] && echo "no jzmq lib found" && exit -1
}

check_zk(){
	zkclient=`zkCli.sh ls / 2>&1|grep ConnectionLossException`
	[ ! -z "$zkclient" ] && echo "please check ZK connection:"$zkclient && exit -1
	echo $zkclient
}

check_env_compile(){
	check_mysql
	check_thrift

	if [ -z "$MVN_HOME" -o -z "$JAVA_HOME" ]; then
		echo "Please set up env in /etc/profile first,example"
		echo " export MVN_HOME=PATH_OF_ANT"
		echo " export JAVA_HOME=PATH_OF_JAVA"
		echo " export PATH=\$PATH:\$MVN_HOME/bin:\$JAVA_HOME/bin"
		#exit -1
	fi
#echo "PATH=$PATH"
#echo "MVN_HOME=$MVN_HOME"
#echo "JAVA_HOME=$JAVA_HOME"
}

check_env_runtime(){
	check_thrift
	check_zk
}

set_compile_flags() {
	export CPPFLAGS="$CPPFLAGS -I/usr/local/include "
	export LDFLAGS="$LDFLAGS -L/usr/local/lib "
}

clean_make () {
	echo "BUILD: start clean make in `pwd`"
	[ -f Makefile ] && make distclean &>/dev/null
	./autogen.sh && ./configure && make
	[ $? -eq 0 ] || exit 1
}

zonda_mvn () {
	echo "BUILD: start mvn in `pwd`"
  mvn install -Dmaven.test.skip -Ptaobao
  [ $? -eq 0 ] || exit 1
  mvn install -DskipTests -Dautoconf.skip -Denv=release -f oam/web-console/pom.xml
  [ $? -eq 0 ] || exit 1
}

zonda_make () {
	echo "BUILD: start make in `pwd`"
	[ ! -f Makefile ] && ./autogen.sh && ./configure
	make
	[ $? -eq 0 ] || exit 1
}

build_redis(){
	REDIS_BIN=$PACKAGE_DIR/vendor/redis
	mkdir -p $REDIS_BIN

	cd 	$TOP_SRCDIR/vendor/redis/redis-2.4.9
	make
	cp -pf src/redis-server $REDIS_BIN/.
	cp -pf src/redis-benchmark $REDIS_BIN/.
	cp -pf src/redis-cli $REDIS_BIN/.
	cp -pf src/redis-check-dump $REDIS_BIN/.
	cp -pf src/redis-check-aof $REDIS_BIN/.
	cp -pf redis.conf $REDIS_BIN/.
}

check_bin_files () {
	cd $TOP_SRCDIR
	echo "BUILD:Verifying check_bin_files for Zonda in `pwd`"
	error=0

	for binfile in $ZONDA_BIN_FILES; do
		if [ ! -f $binfile ]; then
			echo "ERROR: failed to build file: $binfile"
			error=1
		else
			echo "OK: $binfile"
		fi
	done
	[ $error -eq 1 ] && exit 1 || return 0
}

copy_java_files () {
	#list all svn java project to be installed
	srcPaths=(deploy/ZKInit log/server metadata/metadata-server scheduler/server/jobsched-server) # scheduler/server/jobsched-executor
	jarPaths=(ZKInit LogServer MetadataServer SchedulerServer) # SchedulerExecutor

	for ((i=0; i<${#srcPaths[*]}; i++ )); do
		targetDir=$PACKAGE_DIR/jar/${jarPaths[$i]}
	  [ -d $targetDir ] && rm -rf $targetDir
	  mkdir -p $targetDir

	  if [ -f ${srcPaths[$i]}/target/*.tar.gz ]; then
	    tar xvzf ${srcPaths[$i]}/target/*.tar.gz -C $targetDir
		  echo "...... Build ${srcPaths[$i]} to $targetDir success ....."
	  else
		  echo "...... Build ${srcPaths[$i]} to $targetDir failed ....."
		  exit -1
		fi
	done

	tar xvzf oam/web-console/target/web-console.tar.gz -C $PACKAGE_DIR/Web
	cp -f oam/web-console/antx.properties $PACKAGE_DIR/Web
}

copy_files () {
	error=0
	cd $TOP_SRCDIR

	echo "BUILD:Creating tar & copy_files in `pwd`"

	[ -d $PACKAGE_DIR ] && rm -rf $PACKAGE_DIR
	mkdir $PACKAGE_DIR
	touch $PACKAGE_DIR/version-$VERSION
	chmod 444 $PACKAGE_DIR/version-$VERSION

	# Node_sw
	BIN_DIR=$PACKAGE_DIR/bin
	JAR_DIR=$PACKAGE_DIR/jar
	SCHEMA_DIR=$PACKAGE_DIR/schema
	CFG_DIR=$PACKAGE_DIR/config
	WEB_PAGE_DIR=$PACKAGE_DIR/Web
	SCRIPT_DIR=$PACKAGE_DIR/script
	VENDOR_LIB_DIR=$PACKAGE_DIR/vendor/lib
	VENDOR_BIN_DIR=$PACKAGE_DIR/vendor/bin
	VENDOR_VENDOR_DIR=$PACKAGE_DIR/vendor

	dirs=($BIN_DIR $JAR_DIR $SCHEMA_DIR $CFG_DIR $WEB_PAGE_DIR $SCRIPT_DIR $VENDOR_LIB_DIR $VENDOR_BIN_DIR $VENDOR_VENDOR_DIR)
	for subdir in "${dirs[@]}"; do
		[ -d $subdir ] || mkdir -p $subdir
	done

	[ $error -eq 1 ] && exit 1

	for binfile in $ZONDA_BIN_FILES; do
		cp -pf $binfile $BIN_DIR
	done

	for binfile in $ZONDA_VENDOR_LIB; do
		cp -pf $binfile* $VENDOR_LIB_DIR
	done

	for binfile in $ZONDA_VENDOR_BIN; do
		cp -pf $binfile* $VENDOR_BIN_DIR
	done

	cp -f corebiz/dataServer/_log.conf $BIN_DIR

	cp -rpf $TOP_SRCDIR/tools/*.sh $SCRIPT_DIR
	cp -pf $TOP_SRCDIR/deploy/install/* $SCRIPT_DIR
	cp -f $TOP_SRCDIR/vendor/gluster/tools/pre-distribution/zonda_volume-fuse.vol $SCRIPT_DIR
	cp -f $TOP_SRCDIR/vendor/gluster/tools/pre-distribution/zonda_volume-replicate $SCRIPT_DIR

	cp -rf $TOP_SRCDIR/oam/design/{*.sh,*.sql,*.xls} $SCHEMA_DIR

	copy_java_files
	build_redis

	mkdir -p $VENDOR_VENDOR_DIR/zookeeper
	cp -rpf $TOP_SRCDIR/vendor/zookeeper/zookeeper-3.3.4/* $VENDOR_VENDOR_DIR/zookeeper
	find $VENDOR_VENDOR_DIR/zookeeper -type d -print | grep -E "\.svn$" | xargs -i rm -rf {}

	mkdir -p $VENDOR_VENDOR_DIR/timetunnel
	cp -rpf $TOP_SRCDIR/vendor/timetunnel-0.2.0/* $VENDOR_VENDOR_DIR/timetunnel
	find $VENDOR_VENDOR_DIR/timetunnel -type d -print | grep -E "\.svn$" | xargs -i rm -rf {}

	tar xzf $TOP_SRCDIR/vendor/gluster/gluster-bin.tar.gz -C $VENDOR_VENDOR_DIR

	mkdir -p $VENDOR_VENDOR_DIR/mongodb
	cp -rpf $TOP_SRCDIR/vendor/mongodb/package/* $VENDOR_VENDOR_DIR/mongodb
	find $VENDOR_VENDOR_DIR/mongodb -type d -print | grep -E "\.svn$" | xargs -i rm -rf {}

	cd $PACKAGE_DIR/../
	ZONDA_TAR_NAME=zonda-$VERSION-`date +%Y%m%d_%H%M`.tar.gz
	tar czvf $ZONDA_TAR_NAME `echo $PACKAGE_DIR|xargs basename` &>/dev/null
	echo "BUILD:tar file=$ZONDA_TAR_NAME successfully generated." && ls -al $ZONDA_TAR_NAME
}

BUILD_ALL() {
	echo "BUILD:Building Zonda in `pwd`"
	if [ ! -z "$PACKAGE_BUILD" ]; then
		check_bin_files
		copy_files
		echo "BUILD:Requested PACKAGE_BUILD was executed"
		return
	fi

	#env check
	set_compile_flags
	check_env_compile

	# Build
 	[ $? -eq 0 ] && zonda_make
	[ $? -eq 0 ] && zonda_mvn
	# Package
	[ $? -eq 0 ] && check_bin_files
	[ $? -eq 0 ] && copy_files
}

ZONDA_BIN_FILES="
  deploy/cf_init/CFInit
  deploy/cf_init/CFCreate
  deploy/cf_init/CFCheck
  oam/config/test/TestZooKeeper
	route_admin/route_admin
	oam/config/ConfigService
	oam/monitor/agent/MonitorAgent
	oam/counter/service/CounterService
	oam/counter/agent/CounterAgent
	corebiz/dataServer/dataServer
	corebiz/dataServer/importapp
 	corebiz/dataServer/exportapp
	corebiz/indexServer/indexServer
	pm/pm
	migration/cpp/migration
	corebiz/bigfile/agent/server/GlusterServerAgent
	vendor/gluster/tools/pre-distribution/Predistribution
	corebiz/bigfile/logListener/ErrorLogListener
	corebiz/bigfile/predictionServer/BrickPredictionServer
"

ZONDA_VENDOR_BIN="
	/usr/local/bin/plink
	/usr/local/bin/pscp
	/usr/local/bin/psftp
"

ZONDA_VENDOR_LIB="
	/usr/local/lib/liblog4cpp.so
	/usr/local/lib/libzmq.so
	/usr/local/lib/libthrift.so
	/usr/local/lib/libjzmq.so
	/usr/local/lib/libzookeeper_mt.so
	/lib64/libssl.so.6
"

#############################################################
# Main
#############################################################
#[ $# -eq 1 -o $# -eq 2 ] || usage

TOP_DIR=`pwd | xargs dirname`
TOP_SRCDIR=`pwd`
#echo "TOP_DIR=$TOP_DIR"
PACKAGE_DIR=$TOP_DIR/zonda_dist
VERSION=0.1.0
if [ ! $# -eq 0 ]; then
	echo "[dbg mode] invoke only 1 step"
	$1
	exit 0
fi

BUILD_ALL
exit 0

