#pragma once

#include "ProcInfoMgr/ProcInfo.h"

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
	void ClearProcInfos();
	std::vector< ProcInfo* > & GetAllProcInfos();

private:
	std::vector< ProcInfo* > m_vecInfoReusePool;
	std::vector< ProcInfo* > m_vecProcInfos;
};
