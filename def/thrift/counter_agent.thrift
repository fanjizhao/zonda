namespace cpp zonda.msg.counter_agent.thrift
namespace java com.alibaba.platform.zonda.msg.counter_agent.thrift

struct SendCounterReq {
  1: string table_name,
  2: string key1,
  3: string key2,
  4: string key3,
  5: string column_name,
  6: i64 value
}
struct SendCounterRes {
  1: i16 ret_code
}

struct Item {
  1: string table_name,
  2: string key1,
  3: string key2,
  4: string key3,
  5: string column_name,
  6: i64 value
}
struct SendBatchCounterReq {
  1: list<Item> itemList
}
struct SendBatchCounterRes {
  1: i16 ret_code
}
