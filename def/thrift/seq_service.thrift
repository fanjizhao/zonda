namespace cpp zonda.msg.seq_service.thrift
namespace java com.alibaba.platform.zonda.msg.seq_service.thrift

struct GetSeqReq
{
    1:i16 seq_type;
    2:i32 count;
}

struct GetSeqRes
{
    1:i32 start;
    2:i32 count;
}
