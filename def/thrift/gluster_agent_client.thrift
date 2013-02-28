namespace cpp  zonda.msg.gluster_agent_client.thrift
namespace java com.alibaba.platform.zonda.msg.gluster_agent_client.thrift

struct MountVolumeReq {
    1:string path,
    2:string config_server_ip,
    3:string config_server_port,
    4:string volume,
    5:string log_level,
    6:string log_file
}

struct MountVolumeRes {
    1:i16 ret_code
}

struct UnmountVolumeReq {
    1:string path,
    2:i16 force
}

struct UnmountVolumeRes {
    1:i16 ret_code
}

struct GetProcessReq {

}

struct GetProcessRes {
    1:list<string> processes
}

struct GetPidReq {

}

struct GetPidRes {
    1:string pid
}
