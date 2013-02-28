namespace cpp zonda.msg.brick_prediction_service.thrift
namespace java com.alibaba.platform.zonda.msg.brick_prediction_service.thrift

struct PutUnfixedBrickReq
{
    1:string volname
    2:string cfname;
}

struct PutUnfixedBrickRsp
{
    1:list<string> bricks;
    2:i32 errorCode;
}

struct PutFixedBrickReq
{
    1:string volname
    2:string cfname;
    3:string brick;
}

struct PutFixedBrickRsp
{
    1:string result;
    2:i32 errorCode
}