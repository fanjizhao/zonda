PID=`ps ax|grep "./client" |grep -v grep |awk '{print $1}'`
#echo $PID
top -p $PID -H
