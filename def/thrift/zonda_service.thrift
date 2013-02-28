namespace cpp zonda.msg.zonda_service.thrift
namespace java com.alibaba.platform.zonda.msg.zonda_service.thrift

struct ReadReq {
  1: string ns,
  2: string fk
}

struct ReadRsp {
  1: i32 cf,
  2: i32 ds,
  3: i32 errorCode 
}

struct WriteReq {
  1: string ns,
  2: string fk,
  3: string fn,
  4: bool overwrite
}

struct WriteRsp {
  1: i32 cf,
  2: i32 ds,
  3: i32 errorCode 
}

struct DeleteReq {
  1: string ns,
  2: string fk
}

struct DeleteRsp {
  1: i32 errorCode 
}
