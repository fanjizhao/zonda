#!/bin/sh
echo delete all child under /zonda/runtime ...
../oam/config/test/TestZooKeeper /zonda/runtime
#echo delete all child under /zonda/route_admin ...
#../oam/config/test/TestZooKeeper /zonda/route_admin
#echo update /zonda/route_strategy ...
#../oam/config/test/TestZooKeeper /zonda/route_strategy "1=4\n2=6\n3=5\n4=6\n7=6\n8=6"
#echo clear /zonda/route_table
#../oam/config/test/TestZooKeeper /zonda/route_table ""

