$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh delete /zonda/route_table
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh delete /zonda/route_strategy
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh delete /zonda/runtime
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh delete /zonda/route_admin
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh delete /zonda/conf/bigfile/vol_lay_path
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh delete /zonda/conf/bigfile/logListener/conversion
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh delete /zonda/conf/bigfile/logListener/exclusion
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh delete /zonda/conf/bigfile/logListener
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh delete /zonda/conf/bigfile
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh delete /zonda/conf/dragoon
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh delete /zonda/conf
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh delete /zonda/service_grouping
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh delete /zonda

$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh create /zonda "#"
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh create /zonda/route_strategy "#"
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh create /zonda/runtime "#"
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh create /zonda/route_admin "#"
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh create /zonda/conf "#"
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh create /zonda/conf/dragoon "#"
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh create /zonda/conf/bigfile "#"
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh create /zonda/conf/bigfile/vol_lay_path "zonda_volume=../script/:../script/"
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh create /zonda/conf/bigfile/logListener "../vendor/gluster/local/log/glusterfs"
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh create /zonda/conf/bigfile/logListener/conversion "#"
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh create /zonda/conf/bigfile/logListener/exclusion "#"
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh create /zonda/route_table "#"
$ZONDA_HOME/vendor/zookeeper/bin/zkCli.sh create /zonda/service_grouping "#"

$ZONDA_HOME/bin/TestZooKeeper /zonda/route_strategy "1=4\n2=6\n3=5\n4=4\n6=1\n7=6\n8=6\n10=4\n12=6"
$ZONDA_HOME/bin/TestZooKeeper /zonda/conf/dragoon "dragoon_path=/usr/alisys/dragoon\ndragoon_ip=10.20.149.113\ndragoon_port=13888"

grep -v "<host-name>" $ZONDA_HOME/jar/ZKInit/conf/ZKData.xml > /tmp/zk.tmp
hosts_lineno=`cat /tmp/zk.tmp |awk '/<hosts>/{print NR}'`
awk "NR<=$hosts_lineno" /tmp/zk.tmp > /tmp/zk.part1.tmp
awk "NR>$hosts_lineno" /tmp/zk.tmp > /tmp/zk.part3.tmp
cat $ZONDA_DEPLOY/hostsnames |sed 's/^/\t\t\t<host-name>/g' |sed 's/$/<\/host-name>/g' > /tmp/zk.part2.tmp
cat /tmp/zk.part1.tmp /tmp/zk.part2.tmp /tmp/zk.part3.tmp > $ZONDA_HOME/jar/ZKInit/conf/ZKData.xml
rm -f /tmp/zk*.tmp

. $ZONDA_HOME/jar/ZKInit/bin/run.sh
