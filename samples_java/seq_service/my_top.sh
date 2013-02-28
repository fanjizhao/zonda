PID=`jps|grep SeqService |awk '{print $1}'`
#echo $PID
jstack -l $PID > log/SeqService_stack.txt
top -p $PID -H
