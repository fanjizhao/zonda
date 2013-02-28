PID=`ps ax|grep "./echo_service" |grep -v grep |awk '{print $1}'`
#echo $PID
top -p $PID -H
