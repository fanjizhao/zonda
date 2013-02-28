#ifndef ZONDA_COMMON_CALL_OUT_RECORD_H_
#define ZONDA_COMMON_CALL_OUT_RECORD_H_


#include <tr1/unordered_map>
#include "thread/ThreadLock.h"
#include "RpcEvent.h"
#include "log/Logger.h"

namespace zonda
{
namespace common
{

using namespace std;
using namespace std::tr1;    
    
struct CallOutRecordItem
{
    CallOutRecordItem():req(NULL) {}
    RpcHead* rpc_head;
    time_t  send_time;
    int timeout;
    IMsg* req;
};

   
 
class CallOutRecord
{
public:
    int add(const CallOutRecordItem& item);
    //! Removed successfully, return 0, not found record, return -1
    int remove(int64_t call_id, IMsg* &req);
    int remove(int64_t call_id, bool free_rpc_head=true);
    int handle_timeout_items();
    size_t size();
private:
    unordered_map<int64_t, CallOutRecordItem>  m_map;
    ThreadLock m_lock;    
};


} //end namespace common
} //end namespace zonda


#endif //ZONDA_COMMON_CALL_OUT_RECORD_H_

