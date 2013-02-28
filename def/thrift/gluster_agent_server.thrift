namespace cpp  zonda.msg.gluster_agent_server.thrift
namespace java com.alibaba.platform.zonda.msg.gluster_agent_server.thrift

struct GlusterTopStatistics {
    1:string brick_name,
    2:string operation,
    3:i16 current_fd_num,
    4:i16 max_fd_num,
    5:string entry_list
}

struct StartGlusterReq {

}

struct StartGlusterRes {
    1:i16 ret_code
}

struct StopGlusterReq {

}

struct StopGlusterRes {
    1:i16 ret_code
}

struct StateGlusterReq {

}

struct StateGlusterRes {
    1:i16 ret_code
}

struct ProbeGlusterNodeReq {
    1:string server_ip
}

struct ProbeGlusterNodeRes {
    1:i16 ret_code
}

struct DetachGlusterNodeReq {
    1:string server_ip
}

struct DetachGlusterNodeRes {
    1:i16 ret_code
}

struct StateGlusterNodesReq {
    
}

struct StateGlusterNodesRes {
    1:string status
}

struct StateVolumesReq {
    
}

struct StateVolumesRes {
    1:string status
}

struct StateVolumeReq {
    1:string volume
}

struct StateVolumeRes {
    1:string status
}

struct CreateVolumeReq {
    1:string volume,
    2:string replica,
    3:string transport,
    4:list<string> bricks
}

struct CreateVolumeRes {
    1:i16 ret_code
}

struct SetVolumeOptionReq {
    1:string volume,
    2:string option,
    3:string value
}

struct SetVolumeOptionRes {
    1:i16 ret_code
}

struct DeleteVolumeReq {
    1:string volume
}

struct DeleteVolumeRes {
    1:i16 ret_code
}

struct StartVolumeReq {
    1:string volume
}

struct StartVolumeRes {
    1:i16 ret_code
}

struct StopVolumeReq {
    1:string volume
}

struct StopVolumeRes {
    1:i16 ret_code
}

struct AddBricksReq {
    1:string volume,
    2:list<string> bricks
}

struct AddBricksRes {
    1:i16 ret_code
}

struct RemoveBricksReq {
    1:string volume,
    2:list<string> bricks
}

struct RemoveBricksRes {
    1:i16 ret_code
}

struct StartRebalanceReq {
    1:string volume,
    2:string operation
}

struct StartRebalanceRes {
    1:i16 ret_code
}

struct StopRebalanceReq {
    1:string volume,
    2:string operation
}

struct StopRebalanceRes {
    1:i16 ret_code
}

struct StateRebalanceReq {
    1:string volume,
    2:string operation
}

struct StateRebalanceRes {
    1:string status
}

struct SetVolumeLogFileReq {
    1:string volume,
    2:string log_file
}

struct SetVolumeLogFileRes {
    1:i16 ret_code
}

struct SetBrickLogFileReq {
    1:string volume,
    2:string brick,
    3:string log_file
}

struct SetBrickLogFileRes {
    1:i16 ret_code
}

struct GetConfigFileListReq {
    1:string etc_dir,
    2:string volume
}

struct GetConfigFileListRes {
    1:list<string> file_list 
}

struct GetProcessListReq {
    
}

struct GetProcessListRes {
    1:list<string> proc_list
}

struct GetProcessReq {
    1:string volume
}

struct GetProcessRes {
    1:list<string> proc_list
}

struct TopOpenReq {
    1:string volume
}

struct TopOpenRes {
    1:list<GlusterTopStatistics> statistics
}

struct GetPidReq {

}

struct GetPidRes {
    1:string pid
}
