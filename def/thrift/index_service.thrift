namespace cpp zonda.msg.index_service.thrift
namespace java com.alibaba.platform.zonda.msg.index_service.thrift

struct ReadReq {
  1: string ns,
  2: binary fk
}

struct ReadRsp {
  1: string cf,
  2: string ds,
  3: i32 errorCode
}

struct WriteReq {
  1: string ns,
  2: binary fk,
  3: string fn,
  4: bool overwrite
}

struct WriteRsp {
  1: string cf,
  2: string ds,
  3: i32 errorCode
}

struct DeleteReq {
  1: string ns,
  2: binary fk
}

struct DeleteRsp {
  1: string cf,
  2: string ds,
  3: i32 errorCode
}
