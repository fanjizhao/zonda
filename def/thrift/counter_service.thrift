namespace cpp zonda.msg.counter_service.thrift
namespace java com.alibaba.platform.zonda.msg.counter_service.thrift

struct Row {
  1: i64 collect_start,
  2: i64 collect_end,
  3: string key1,
  4: string key2,
  5: string key3,
  6: map<string, i64> column_values
}
struct SendCounterAgentReq {
  1: string table_name,
  2: list<Row> row
}
struct SendCounterAgentRes {
  1: i16 ret_code
}

struct SendAlarmReq {
  1: i64 alarm_time,
  2: i64 alarm_id,
  3: string alarm_module,
  4: string brief_desc,
  5: string detail_desc
}
struct SendAlarmRes {
  1: i16 ret_code
}
