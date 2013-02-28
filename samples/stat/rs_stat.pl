open(C_SEND, "<", "c_send.txt");
open(S_RECV, "<", "s_recv.txt");
open(S_SEND, "<", "s_send.txt");
open(C_RECV, "<", "c_recv.txt");

$sum_req_trans_time = 0;
$sum_req_handle_time = 0;
$sum_res_trans_time = 0;
$req_num = 0;
while ($line = <C_SEND>)
{
    $req_num++;
    chomp($line);
    ($call_id, $c_send_time) = split(/\t/, $line);
    
    $line = $call_id."  ".$c_send_time;
    
    
    $line2 = <S_RECV>;
    chomp($line2);
    ($call_id, $s_recv_time) = split(/\t/, $line2);
    $line .= "  ".$s_recv_time;
    
    $line2 = <S_SEND>;
    chomp($line2);
    ($call_id, $s_send_time) = split(/\t/, $line2);
    $line .= "  ".$s_send_time;
    
    $line2 = <C_RECV>;
    chomp($line2);
    ($call_id, $c_recv_time) = split(/\t/, $line2);
    $line .= "  ".$c_recv_time;

    $req_trans_time =  $s_recv_time - $c_send_time;
    $req_handle_time = $s_send_time - $s_recv_time;
    $res_trans_time = $c_recv_time - $s_send_time;
    
    $sum_req_trans_time += $req_trans_time;
    $sum_req_handle_time += $req_handle_time;
    $sum_res_trans_time += $res_trans_time;
    #printf("%s  %.6f  %.6f  %.6f\n", $line, $req_trans_time, $req_handle_time, $res_trans_time);
}

print "rpc call: $req_num\n";
print "sum_req_trans_time: $sum_req_trans_time\n";
print "sum_req_handle_time: $sum_req_handle_time\n";
print "sum_res_trans_time: $sum_res_trans_time\n";

printf("avg_req_trans_time: %.6f\n", ($sum_req_trans_time/$req_num));
printf("avg_req_handle_time: %.6f\n", $sum_req_handle_time/$req_num);
printf("avg_res_trans_time: %.6f\n", $sum_res_trans_time/$req_num);
printf("avg_trans_time: %.6f\n", ($sum_req_trans_time+$sum_res_trans_time)/$req_num);