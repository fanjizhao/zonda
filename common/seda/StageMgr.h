#ifndef ZONDA_COMMON_STAGEMGR_H_
#define ZONDA_COMMON_STAGEMGR_H_

#include <vector>
#include <tr1/unordered_map>
#include "IStage.h"
#include "util/Singleton.h"
#include "thread/ThreadLock.h"


namespace zonda
{    
namespace common
{

using namespace std;
using namespace std::tr1;   
    
//! Singleton
class StageMgrImpl
{
public:
    IStage* get_stage(const char* stage_name);
    IStage* get_stage(uint16_t stage_id);
    int reg_stage(IStage* stage);
    int reg_stage(IStage* stage, uint16_t& stage_id);
    int dereg_stage(IStage* stage);
    void stop_all();

protected:
	StageMgrImpl();
	int do_reg_stage(IStage* stage, uint16_t& stage_id);
	
private:
	typedef unordered_map<string, IStage*> STAGE_MAP;
	STAGE_MAP m_stage_map;
	vector<IStage*> m_stage_list;
	ThreadLock m_lock;
    
};

typedef Singleton<StageMgrImpl, ThreadLock> StageMgr;

}//end namespace server_platform
}//end namespace zonda
#endif //ZONDA_COMMON_STAGEMGR_H_
