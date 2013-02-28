PID=`jps|grep Client |awk '{print $1}'`
#echo $PID
jstack -l $PID > log/Client_stack.txt
top -p $PID -H
