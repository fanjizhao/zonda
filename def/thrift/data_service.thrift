namespace cpp zonda.msg.data_service.thrift
namespace java com.alibaba.platform.zonda.msg.data_service.thrift

#@offset: offset in small file.
#@len:    pieces length of the requesting small file. <0 means read all.
#
struct ReadReq {
  1: string ns,
  2: binary fk,
  3: string fn,
  4: string cf,
  5: i32 offset,
  6: i32 len
}

struct ReadRsp {
  1: bool isEnd,
  2: i32 errorCode,
  3: i32 len,
  4: binary content
}

struct WriteReq {
  1: string ns,
  2: binary fk,
  3: string fn,
  4: string cf,
  5: bool overwrite,
  6: binary content,
  7: bool isEnd
}

struct WriteRsp {
  1: i32 len,
  2: i32 errorCode
}

struct DeleteReq {
  1: string ns,
  2: binary fk,
  3: string fn,
  4: string cf
}

struct DeleteRsp {
  1: i32 errorCode
}
