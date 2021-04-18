#pragma once

#include "ProcInfoMgr/ProcInfo.h"
#include "include/kbase/lock.h"

class DataCenter
{
private:
	DataCenter(void);

public:
	~DataCenter(void);

	static DataCenter* GetInstance();

	void ReuseProcInfo(ProcInfo* pInfo);
	ProcInfo* CreateProcInfo();
	void AddProcInfo(ProcInfo* pInfo);
	ProcInfo* GetProcInfo(DWORD dwPid);
	void ClearProcInfos();
	void GetAllProcInfos(std::vector< ProcInfo* > & vecProcInfos);
	int GetProcCount();

private:
	std::vector< ProcInfo* > m_vecInfoReusePool;
	std::map< DWORD, ProcInfo* > m_mapPid2ProcInfos;
    kbase::Locker m_lockInfos;
};
