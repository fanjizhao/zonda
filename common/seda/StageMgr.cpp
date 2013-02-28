#include "log/Logger.h"
#include "StageMgr.h"


using namespace zonda::common;
    
StageMgrImpl::StageMgrImpl()
{
}


IStage* StageMgrImpl::get_stage(const char* stage_name)
{
	IStage* stage = NULL;
	m_lock.lock();
	STAGE_MAP::iterator it = m_stage_map.find(stage_name);
	if (it != m_stage_map.end())
	{
		stage = it->second;
	}
	m_lock.unlock();
	
	return stage;
}

IStage* StageMgrImpl::get_stage(uint16_t stage_id)
{
    IStage* stage = NULL;
    m_lock.lock();    
    if (stage_id >= m_stage_list.size())
    {
        m_lock.unlock();
        return NULL;
    }
    
    stage = m_stage_list[stage_id];
    LOG_DEBUG("get_stage id:" << stage_id << ", stage_name:" << stage->get_name());
    m_lock.unlock();
    return stage;
}

int StageMgrImpl::do_reg_stage(IStage* stage, uint16_t& stage_id)
{
	int r = 0;
	
	m_lock.lock();
	STAGE_MAP::iterator it = m_stage_map.find(stage->get_name());
	if (it == m_stage_map.end())
	{
	    stage_id = m_stage_list.size();
	    m_stage_list.push_back(stage);
		m_stage_map[stage->get_name()] = stage;
		LOG_INFO("stage: " << stage->get_name() << ", id:" << stage_id);
	}
	else
	{
	    LOG_FATAL("stage:" << stage->get_name() << " duplicated");
		r = -1;
	}
	
	m_lock.unlock();
	
	return r;    
}

int StageMgrImpl::reg_stage(IStage* stage)
{
    uint16_t id;
    return do_reg_stage(stage, id);
}

int StageMgrImpl::reg_stage(IStage* stage, uint16_t& stage_id)
{
    return do_reg_stage(stage, stage_id);
}


int StageMgrImpl::dereg_stage(IStage* stage)
{
    m_lock.lock();
    STAGE_MAP::iterator it = m_stage_map.find(stage->get_name());
    if (it != m_stage_map.end())
    {
    	m_stage_map.erase(it);
    }
    
    m_stage_list[stage->get_id()] = NULL;
    m_lock.unlock();
	
	return 0;
}

void StageMgrImpl::stop_all()
{
	m_lock.lock();
	for(STAGE_MAP::iterator it = m_stage_map.begin();
		it != m_stage_map.end(); ++it)
	{
		IStage* stage = it->second;
		stage->stop();
	}
	m_lock.unlock();
}
    



using namespace zonda::common;

