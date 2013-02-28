PID=`jps|grep EchoService |awk '{print $1}'`
#echo $PID
jstack -l $PID > log/EchoService_stack.txt
top -p $PID -H
