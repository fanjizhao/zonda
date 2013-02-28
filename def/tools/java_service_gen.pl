#!/usr/bin/perl



sub remove_blanks
{
    my ($string) = @_;
    $$string =~ s/^\s+//; #remove leading spaces
    $$string =~ s/\s+$//; #remove trailing spaces
}


$PACKAGE_PREFIX = "com.alibaba.platform.zonda";
$PACKAGE_DIR = "./java_def/src/java/com/alibaba/platform/zonda/";
$THRIFT_IMPORT = "import org.apache.thrift.TBase;\n";
$THRIFT_IMPORT .= "import org.apache.thrift.TDeserializer;\n";
$THRIFT_IMPORT .= "import org.apache.thrift.TException;\n";
$THRIFT_IMPORT .= "import org.apache.thrift.TSerializer;";
$THRIFT_IMPORT .= "import org.apache.thrift.protocol.TBinaryProtocol;\n";
$THRIFT_IMPORT .= "import org.apache.thrift.protocol.TBinaryProtocol.Factory;\n";
$THRIFT_IMPORT .= "import org.apache.thrift.protocol.TJSONProtocol;\n";
$THRIFT_IMPORT .= "import org.apache.thrift.protocol.TProtocol;\n";
$THRIFT_IMPORT .= "import org.apache.thrift.protocol.TProtocolFactory;\n";
$THRIFT_IMPORT .= "import org.apache.thrift.transport.TMemoryBuffer;\n";
$THRIFT_IMPORT .= "import org.apache.thrift.transport.TMemoryInputTransport;\n";
$THRIFT_IMPORT .= "import org.apache.thrift.transport.TTransport;\n";
$THRIFT_IMPORT .= "import org.apache.thrift.transport.TTransportException;\n";
$DECODE_PROTOCOL = "TBinaryProtocol";

$OP_TYPE_RANGE = 100;
my %serv_hash;

open(SERV_DEF, "<", "../def/services.def");
my $current_serv_type = "";

#CLIENT is also a service type
my $service_count = 1;
my @operations = ();
my @req_msg_list = ();
my @res_msg_list = ();
while($line = <SERV_DEF>)
{
    chomp($line);
    if ($line =~ /BEGIN_DECLARE_SERVICE\((.*)\)/ )
    {
        #print $1, "\n";
        ($serv_type, $serv_type_value, $route_strategy) = split(/,/, $1);
        $serv_hash{$serv_type}{"serv_type_value"} = $serv_type_value;
        remove_blanks(\$serv_type);
        $serv_hash{$serv_type}{"name_space"} = lc($serv_type);
        $current_serv_type = $serv_type;
        $service_count++;    
    }
    elsif ($line =~ /DECLARE_OPERATION\((.*)\)/)
    {
        ($op_type, $op_type_seq, $op_req, $op_res) = split(/,/, $1);
        #print $op_type, "\n";
        #print $current_serv_type, "\n";
        $serv_hash{$serv_type}{"operation"}{$op_type}{"op_type"} = $op_type;
        $serv_hash{$serv_type}{"operation"}{$op_type}{"op_type_value"} = $op_type_seq;
        remove_blanks(\$op_req);
        $serv_hash{$serv_type}{"operation"}{$op_type}{"op_req"} = $op_req;
        remove_blanks(\$op_res);
        $serv_hash{$serv_type}{"operation"}{$op_type}{"op_res"} = $op_res;
        
        my $msg_type = ($serv_hash{$serv_type}{"serv_type_value"}*100 +  $op_type_seq);
        $req_msg_list[$msg_type] = "${op_type}_REQ";
        $res_msg_list[$msg_type] = "${op_type}_RES";
        
        
    }
    elsif ($line =~ /END_DECLARE_SERVICE/)
    {
        #$serv_hash{$current_serv_type}{"operations"} = \@operations;
        #@operations = ();
    }    
}

$gen_content = "";
$gen_content .= "package $PACKAGE_PREFIX;\n";
$gen_content .= "public class Service {\n";
$gen_content .= "\tpublic static final int SERVICE_COUNT = $service_count;\n";
$gen_content .= "\tpublic static final String serviceNameList[]={\n";

my @tmp_array = ();
for (keys(%serv_hash))
{
    $tmp_array[$serv_hash{$_}{'serv_type_value'}] = $_;
    
}

$gen_content .= "\t\t\"CLIENT\",\n";

for ($i=1; $i< scalar(@tmp_array); ++$i)
{
    $gen_content .= "\t\t\"".$tmp_array[$i]. "\",\n";
}
$gen_content .= "\t};\n";


$gen_content .= "\tpublic static final String reqNameList[]={\n";

for ($i = 1; $i <$service_count; ++$i)
{
    $line = "\t\t";
    for ($j=0; $j<100; ++$j)
    {
        $line .= "\"".$req_msg_list[$i*100 + $j]."\",";
    }
    $gen_content .= $line;
    $gen_content .= "\n";
}
$gen_content .= "\t};\n";

$gen_content .= "\tpublic static String resNameList[]={\n";
for ($i = 1; $i <$service_count; ++$i)
{
    $line = "\t\t";
    for ($j=0; $j<100; ++$j)
    {
        $line .= "\"".$res_msg_list[$i*100 + $j]."\",";
    }
    $gen_content .= $line;
    $gen_content .= "\n";
}
$gen_content .= "\t};\n";


$gen_content .= "}\n";
#print $gen_content;
$file_name = $PACKAGE_DIR."Service.java";
open(SERVICE_JAVA, ">", $file_name) or die($!." ".$file_name);
print SERVICE_JAVA $gen_content;
close(SERVICE_JAVA);


$gen_content = "";
$gen_content .= "package $PACKAGE_PREFIX;\n";
$gen_content .= "public class ServiceType {\n";
$gen_content .= "\tpublic static final int CLIENT = 0;\n";
for (keys(%serv_hash))
{
    $gen_content .= "\tpublic static final int $_ = $serv_hash{$_}{'serv_type_value'};\n";
}

$gen_content .= "}\n";

$file_name = $PACKAGE_DIR."ServiceType.java";
open(SERVICE_TYPE_JAVA, ">", $file_name) or die($!." ".$file_name);
print SERVICE_TYPE_JAVA $gen_content;
close(SERVICE_TYPE_JAVA);




#produce definitions for op_type


foreach $serv_type (keys(%serv_hash))
{
    $gen_content = "";
    $gen_content .= "package $PACKAGE_PREFIX.op_type;\n";
        

    my $class_name = uc($serv_hash{$serv_type}{"name_space"});
    my $serv_type_value = $serv_hash{$serv_type}{"serv_type_value"};
    $gen_content .= "public class $class_name {\n";
    my %my_hash = %{$serv_hash{$serv_type}{"operation"}};
    my @tmp = ();
    foreach $key (keys(%my_hash))
    {
        #$gen_content .= "\tpublic static final int ".$my_hash{$key}{"op_type"}." = ".($serv_type_value*$OP_TYPE_RANGE+$my_hash{$key}{"op_type_value"}).";\n";
        #$tmp[$serv_type_value*$OP_TYPE_RANGE+$my_hash{$key}{"op_type_value"}] = $my_hash{$key}{"op_type"};
        $tmp[$my_hash{$key}{"op_type_value"}] = $my_hash{$key}{"op_type"};
        #print $my_hash{$key}{"op_type_value"}. "#######".$my_hash{$key}{"op_type"}."\n";
                
    }
    
    for($i=1; $i<scalar(@tmp); ++$i)
    {
        if (length($tmp[$i]) == 0) 
        {
            next;
        }
        $gen_content .= "\tpublic static final int ".$tmp[$i]." = ".($serv_type_value*$OP_TYPE_RANGE+$i).";\n";
    }
    $gen_content .= "}\n";
    
    #print $gen_content;
    $file_name = $PACKAGE_DIR."op_type/$class_name.java";
    open(JAVA_FILE, ">", $file_name) or die($!." ".$file_name);
    print JAVA_FILE $gen_content;
    close(JAVA_FILE);
}



#produce definitions for msg_type
foreach $serv_type (keys(%serv_hash))
{
    $gen_content = "";
    $gen_content .= "package $PACKAGE_PREFIX.msg_type;\n";
    
    my $class_name = uc($serv_hash{$serv_type}{"name_space"});
    my $serv_type_value = $serv_hash{$serv_type}{"serv_type_value"};
    $gen_content .= "public class $class_name {\n";
    my %my_hash = %{$serv_hash{$serv_type}{"operation"}};
    my @tmp = ();
    foreach $key (keys(%my_hash))
    {
        $tmp[$my_hash{$key}{"op_type_value"}] = $my_hash{$key}{"op_type"};
    }
    
    for($i=1; $i<scalar(@tmp); ++$i)
    {
        if (length($tmp[$i]) == 0) 
        {
            next;
        }
        $gen_content .= "\tpublic static final int ".$tmp[$i]."_REQ = ".($serv_type_value*$OP_TYPE_RANGE+$i).";\n";
        $gen_content .= "\tpublic static final int ".$tmp[$i]."_RES = ".(-1*($serv_type_value*$OP_TYPE_RANGE+$i)).";\n";
    }
    $gen_content .= "}\n";
    
    #print $gen_content;
    $file_name = $PACKAGE_DIR."msg_type/$class_name.java";
    open(JAVA_FILE, ">", $file_name) or die($!." ".$file_name);
    print JAVA_FILE $gen_content;
    close(JAVA_FILE);
}



foreach $serv_type (keys(%serv_hash))
{
    $gen_content = "";
        
    my $name_space = $serv_hash{$serv_type}{"name_space"};
    my $serv_type_value = $serv_hash{$serv_type}{"serv_type_value"};

    $gen_content .= "package $PACKAGE_PREFIX.msg.$name_space;\n";
    $gen_content .= $THRIFT_IMPORT;
    $gen_content .= "import $PACKAGE_PREFIX.common.rpc.IMsg;\n";
    

    my %my_hash = %{$serv_hash{$serv_type}{"operation"}};
    #print %my_hash;
    foreach $key (keys(%my_hash))
    {
        #Create the wrapper for XXXReq
        $thrift_msg = "$PACKAGE_PREFIX.msg.$name_space.thrift.".$my_hash{$key}{"op_req"};
        $imsg = "IMsg";
        $new_msg = $my_hash{$key}{"op_req"};
        $msg_type_value = $serv_type_value*$OP_TYPE_RANGE + $my_hash{$key}{"op_type_value"};
        $content = "public class $new_msg extends $thrift_msg implements $imsg\n{\n";
        $content .= "\tpublic int getMsgType() {return $msg_type_value;}\n";
        $content .= "\tpublic int getOpType() {return $msg_type_value;}\n";
        $content .= "\tpublic byte[] serialize() throws Exception {\n";
        $content .= "\t\tTSerializer ts = new TSerializer(new $DECODE_PROTOCOL.Factory());\n";
        $content .= "\t\treturn ts.serialize((TBase)this);\n";
        $content .= "\t}\n";
        $content .= "\tpublic void deserialize(byte[] buff, int pos, int len) throws Exception {\n";
        $content .= "\t\tTTransport trans = new TMemoryInputTransport(buff, pos, len);\n";
        $content .= "\t\tTProtocol proto = new $DECODE_PROTOCOL(trans);\n";
        $content .= "\t\tthis.read(proto);\n";
        $content .= "\t}\n";
        $content .= "}\n";
        #print $gen_content;
        $dir = $PACKAGE_DIR."msg/$name_space";
        if (! -d $dir)
        {
            mkdir($dir);
        }
        $file_name = "$dir/$new_msg.java";
        
        open(JAVA_FILE, ">", $file_name) or die($!." ".$file_name);
        print JAVA_FILE $gen_content;
        print JAVA_FILE $content;
        close(JAVA_FILE);        
        
      
        #Create the wrapper for XXXRes
        $thrift_msg = "$PACKAGE_PREFIX.msg.$name_space.thrift.".$my_hash{$key}{"op_res"};
        $imsg = "IMsg";
        $new_msg = $my_hash{$key}{"op_res"};
        $msg_type_value = -1* ($serv_type_value*$OP_TYPE_RANGE + $my_hash{$key}{"op_type_value"});
        $content = "public class $new_msg extends $thrift_msg implements $imsg\n{\n";
        $content .= "\tpublic int getMsgType() {return $msg_type_value;}\n";
        $content .= "\tpublic int getOpType() {return ".(0-$msg_type_value).";}\n";
        $content .= "\tpublic byte[] serialize() throws Exception {\n";
        $content .= "\t\tTSerializer ts = new TSerializer(new $DECODE_PROTOCOL.Factory());\n";
        $content .= "\t\treturn ts.serialize((TBase)this);\n";
        $content .= "\t}\n";
        $content .= "\tpublic void deserialize(byte[] buff, int pos, int len) throws Exception {\n";
        $content .= "\t\tTTransport trans = new TMemoryInputTransport(buff, pos, len);\n";
        $content .= "\t\tTProtocol proto = new $DECODE_PROTOCOL(trans);\n";
        $content .= "\t\tthis.read(proto);\n";
        $content .= "\t}\n";
        $content .= "}\n";
        #print $gen_content;
        $file_name = "$dir/$new_msg.java";
        open(JAVA_FILE, ">", $file_name) or die($!." ".$file_name);
        print JAVA_FILE $gen_content;
        print JAVA_FILE $content;
        close(JAVA_FILE);        
        
    }
    

}



########Gen MsgFactory.java##########

$gen_content = "";
$gen_content .= "package $PACKAGE_PREFIX;\n";
$gen_content .= "import $PACKAGE_PREFIX.common.rpc.*;\n";
$gen_content .= "import $PACKAGE_PREFIX.Service;\n";
$gen_content .= "public class MsgFactory implements IMsgFactory\n";
$gen_content .= "{\n";
$gen_content .= "\tpublic MsgFactory()\n";
$gen_content .= "\t{\n";
$gen_content .= "\t\tGlobalServicesInfo.getInstance().setServiceCount(Service.SERVICE_COUNT);\n";
$gen_content .= "\t\tGlobalServicesInfo.getInstance().setReqName(Service.reqNameList);\n";
$gen_content .= "\t\tGlobalServicesInfo.getInstance().setResName(Service.resNameList);\n";
$gen_content .= "\t\tGlobalServicesInfo.getInstance().setServiceName(Service.serviceNameList);\n";
$gen_content .= "\t}\n";
$gen_content .= "\tpublic byte[] serialize_body(IMsg msg) throws Exception\n";
$gen_content .= "\t{\n";
$gen_content .= "\t\treturn msg.serialize();\n";
$gen_content .= "\t}\n";
$gen_content .= "\tpublic IMsg deserialize_body(byte[] buff, int pos, int len, int msgType) throws Exception\n";
$gen_content .= "\t{\n";
$gen_content .= "\t\tif (buff == null) throw new Exception(\"buff is null\");\n";
$gen_content .= "\t\tIMsg msg = null;\n";
$gen_content .= "\t\tswitch(msgType)\n";
$gen_content .= "\t\t{\n";

foreach $serv_type (keys(%serv_hash))
{
    my $name_space = $serv_hash{$serv_type}{"name_space"};
    my $serv_type_value = $serv_hash{$serv_type}{"serv_type_value"};

    my %my_hash = %{$serv_hash{$serv_type}{"operation"}};
    #print %my_hash;
    foreach $key (keys(%my_hash))
    {
        #print "\tstatic int ", $my_hash{$key}{"op_type"}, " = ", $my_hash{$key}{"op_type_value"}, ";\n";
        $new_msg = "$PACKAGE_PREFIX.msg.${name_space}.".$my_hash{$key}{"op_req"};
        $msg_type_value = $serv_type_value*$OP_TYPE_RANGE + $my_hash{$key}{"op_type_value"};
        #$msg_type = "zonda::msg_type::".$name_space."::".$my_hash{$key}{"op_type"}."_REQ";
        $gen_content .= "\t\t\tcase $msg_type_value:\n";
        $gen_content .= "\t\t\t\tmsg = new $new_msg();\n";
        $gen_content .= "\t\t\t\tmsg.deserialize(buff, pos, len);\n";
        $gen_content .= "\t\t\t\treturn msg;\n";
         
        
        $new_msg = "$PACKAGE_PREFIX.msg.${name_space}.".$my_hash{$key}{"op_res"};
        $msg_type_value = 0 - $msg_type_value;
        $gen_content .= "\t\t\tcase $msg_type_value:\n";
        $gen_content .= "\t\t\t\tmsg = new $new_msg();\n";
        $gen_content .= "\t\t\t\tmsg.deserialize(buff, pos, len);\n";
        $gen_content .= "\t\t\t\treturn msg;\n";        
    }
}
$gen_content .= "\t\t\tdefault:\n";
$gen_content .= "\t\t\t\tthrow new Exception(\"Unknow msg type:\" + msgType);\n";
$gen_content .= "\t\t}//end switch\n";
$gen_content .= "\t}//end deserialize\n";
$gen_content .= "}\n";



#print $gen_content;

$file_name = $PACKAGE_DIR."MsgFactory.java";
open(JAVA_FILE, ">", $file_name) or die($!." ".$file_name);
print JAVA_FILE $gen_content;
close(JAVA_FILE); 





