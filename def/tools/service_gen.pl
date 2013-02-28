#!/usr/bin/perl



sub remove_blanks
{
    my ($string) = @_;
    $$string =~ s/^\s+//; #remove leading spaces
    $$string =~ s/\s+$//; #remove trailing spaces
}


$OP_TYPE_RANGE = 100;
my %serv_hash;

open(SERV_H, "+>", "../inc/services.h") or die("Failed to open services.h");
open(SERV_CPP, "+>", "../def/services.cpp");
open(SERV_DEF, "<", "../def/services.def");
open(CLASS_MSG, "+>", "../def/class_msg_type-inc.h"); 
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

sub gen_cpp_def_header
{
    print SERV_H "#ifndef ZONDA_INC_SERVICE_H_", "\n";
    print SERV_H "#define ZONDA_INC_SERVICE_H_", "\n";
    
    print SERV_H "#include <stddef.h>", "\n";
    print SERV_H "#include <stdint.h>", "\n";
    print SERV_H '#include "rpc/MsgThrift.h"', "\n";
    
    for (keys(%serv_hash))
    {
        print SERV_H '#include "../def/gen-cpp/'.lc($_)."_types.h", "\"\n";
    }    
    
    
    print SERV_H "namespace zonda", "\n";
    print SERV_H "{", "\n";
    

}

sub gen_cpp_def_footer
{
    print SERV_H "} //namespace zonda", "\n\n";
    print SERV_H "#endif //ZONDA_INC_SERVICE_H_", "\n";
}


gen_cpp_def_header();

print SERV_H "\tstatic const uint16_t SERVICE_COUNT = $service_count;\n";
print SERV_H "\textern const char* service_name_list[".($service_count+1)."];\n";
print SERV_H "\textern const char* req_name_list[".($service_count*100 +1)."];\n";
print SERV_H "\textern const char* res_name_list[".($service_count*100+1)."];\n";
print SERV_H "namespace service_type", "\n";
print SERV_H "{", "\n";
print SERV_H "\tstatic const uint16_t CLIENT = 0;\n";
for (keys(%serv_hash))
{
    print SERV_H "\tstatic const uint16_t $_ = $serv_hash{$_}{'serv_type_value'};", "\n";
}


print SERV_H "} //namespace service_type", "\n\n";


#produce definitions for op_type
print SERV_H "namespace op_type", "\n";
print SERV_H "{", "\n";

foreach $serv_type (keys(%serv_hash))
{
    my $name_space = $serv_hash{$serv_type}{"name_space"};
    my $serv_type_value = $serv_hash{$serv_type}{"serv_type_value"};
    print SERV_H "namespace $name_space", "\n";
    print SERV_H "{", "\n";
    my %my_hash = %{$serv_hash{$serv_type}{"operation"}};
    #print %my_hash;
    foreach $key (keys(%my_hash))
    {
        print SERV_H "\tstatic const int ", $my_hash{$key}{"op_type"}, " = ", $serv_type_value*$OP_TYPE_RANGE+$my_hash{$key}{"op_type_value"}, ";\n";
    }
    print SERV_H "} //namespace $name_space", "\n\n"
}

print SERV_H "} //namespace op_type", "\n\n";





#produce definitions for msg_type
print SERV_H "namespace msg_type", "\n";
print SERV_H "{", "\n";


foreach $serv_type (keys(%serv_hash))
{
    my $name_space = $serv_hash{$serv_type}{"name_space"};
    my $serv_type_value = $serv_hash{$serv_type}{"serv_type_value"};
    print SERV_H "namespace $name_space", "\n";
    print SERV_H "{", "\n";
    my %my_hash = %{$serv_hash{$serv_type}{"operation"}};
    #print %my_hash;
    foreach $key (keys(%my_hash))
    {
        #print "\tstatic int ", $my_hash{$key}{"op_type"}, " = ", $my_hash{$key}{"op_type_value"}, ";\n";
        print SERV_H "\tstatic const int ", $my_hash{$key}{"op_type"}, "_REQ = ", $serv_type_value*$OP_TYPE_RANGE+ $my_hash{$key}{"op_type_value"}, ";\n";
        print SERV_H "\tstatic const int ", $my_hash{$key}{"op_type"}, "_RES = -", $serv_type_value*$OP_TYPE_RANGE+ $my_hash{$key}{"op_type_value"}, ";\n";
        
    }
    print SERV_H "} //namespace $name_space", "\n\n"
}

print SERV_H "} //namespace msg_type", "\n\n";

print SERV_H "namespace msg", "\n";
print SERV_H "{", "\n";


foreach $serv_type (keys(%serv_hash))
{
    my $name_space = $serv_hash{$serv_type}{"name_space"};
    my $serv_type_value = $serv_hash{$serv_type}{"serv_type_value"};
    print SERV_H "namespace $name_space", "\n";
    print SERV_H "{", "\n";
    my %my_hash = %{$serv_hash{$serv_type}{"operation"}};
    #print %my_hash;
    foreach $key (keys(%my_hash))
    {
        #print "\tstatic int ", $my_hash{$key}{"op_type"}, " = ", $my_hash{$key}{"op_type_value"}, ";\n";
        $msg = "zonda::msg::".${name_space}."::thrift::".$my_hash{$key}{"op_req"};
        $new_msg = $my_hash{$key}{"op_req"};
        $msg_type = "zonda::msg_type::".$name_space."::".$my_hash{$key}{"op_type"}."_REQ";
        print SERV_H "\ttypedef zonda::common::MsgThrift<$msg, $msg_type> $new_msg;\n";
        
        $msg = "zonda::msg::".${name_space}."::thrift::".$my_hash{$key}{"op_res"};
        $new_msg = $my_hash{$key}{"op_res"};
        $msg_type = "zonda::msg_type::".$name_space."::".$my_hash{$key}{"op_type"}."_RES";
        print SERV_H "\ttypedef zonda::common::MsgThrift<$msg, $msg_type> $new_msg;\n";        
        
    }
    print SERV_H "} //namespace $name_space", "\n\n"
}


print SERV_H "} //namespace msg", "\n\n";






gen_cpp_def_footer();



#######################Gen services.cpp##########
print SERV_CPP "#include \"services.h\"\n\n";

print SERV_CPP "namespace zonda\n";
print SERV_CPP "{\n";

my @tmp_array = ();
for (keys(%serv_hash))
{
    $tmp_array[$serv_hash{$_}{'serv_type_value'}] = $_;
    
}

print SERV_CPP "const char* service_name_list[]={", "\n";
print SERV_CPP "\t\"CLIENT\",", "\n";

for ($i=1; $i< scalar(@tmp_array); ++$i)
{
    print SERV_CPP "\t\"".$tmp_array[$i]. "\",\n";
}
print SERV_CPP "\tNULL};", "\n\n";



print SERV_CPP "const char* req_name_list[]={", "\n";
for ($i = 0; $i<scalar(@req_msg_list); ++$i)
{
    if (length($req_msg_list[$i]) == 0)
    {
        print SERV_CPP "\t\"\",\n";
    }
    else
    {
        print SERV_CPP "\t\"".$req_msg_list[$i]."\",\n";
    }
}
print SERV_CPP "\tNULL};", "\n\n";

print SERV_CPP "const char* res_name_list[]={", "\n";
for ($i = 0; $i<scalar(@res_msg_list); ++$i)
{
    if (length($res_msg_list[$i]) == 0)
    {
        print SERV_CPP "\t\"\",\n";
    }
    else
    {
        print SERV_CPP "\t\"".$res_msg_list[$i]."\",\n";
    }
}
print SERV_CPP "\tNULL};", "\n\n";

print SERV_CPP "} //namespace zonda \n\n";


########Gen class_msg_type-inc.h##########
foreach $serv_type (keys(%serv_hash))
{
    my $name_space = $serv_hash{$serv_type}{"name_space"};
    my $serv_type_value = $serv_hash{$serv_type}{"serv_type_value"};

    my %my_hash = %{$serv_hash{$serv_type}{"operation"}};
    #print %my_hash;
    foreach $key (keys(%my_hash))
    {
        #print "\tstatic int ", $my_hash{$key}{"op_type"}, " = ", $my_hash{$key}{"op_type_value"}, ";\n";
        $new_msg = "zonda::msg::".${name_space}."::".$my_hash{$key}{"op_req"};
        $msg_type = "zonda::msg_type::".$name_space."::".$my_hash{$key}{"op_type"}."_REQ";
        print CLASS_MSG "_MSG_TYPE_DEAL_FUN($new_msg, $msg_type);\n";
        
        $new_msg = "zonda::msg::".${name_space}."::".$my_hash{$key}{"op_res"};
        $msg_type = "zonda::msg_type::".$name_space."::".$my_hash{$key}{"op_type"}."_RES";
        print CLASS_MSG "_MSG_TYPE_DEAL_FUN($new_msg, $msg_type);\n";        
        
    }
}








