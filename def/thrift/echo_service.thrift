namespace cpp zonda.msg.echo_service.thrift
namespace java com.alibaba.platform.zonda.msg.echo_service.thrift

struct EchoExReq
{
    1:string str;
}

struct EchoExRes
{
    1:string result;
    2:i32 times;
    3:i16 ret_code;
}

struct EchoReq
{
    1:string str;
}

struct EchoRes
{
    1:string str;
}
