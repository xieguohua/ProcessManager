#include "StdAfx.h"
#include "DataCenter.h"

DataCenter::DataCenter(void)
{
}

DataCenter::~DataCenter(void)
{
}

DataCenter* DataCenter::GetInstance()
{
	static DataCenter inst;
	return &inst;
}

void DataCenter::ReuseProcInfo(ProcInfo* pInfo)
{
	if (pInfo != NULL)
	{
		m_vecInfoReusePool.push_back(pInfo);
	}
}

ProcInfo* DataCenter::CreateProcInfo()
{
	ProcInfo* pInfo = NULL;
	if (m_vecInfoReusePool.size() > 0)
	{
		pInfo = m_vecInfoReusePool[m_vecInfoReusePool.size() - 1];
		m_vecInfoReusePool.pop_back();
	}
	else
	{
		pInfo = new ProcInfo();
	}

	return pInfo;
}

void DataCenter::AddProcInfo(ProcInfo* pInfo)
{
	if (pInfo != NULL)
	{
		m_mapPid2ProcInfos[pInfo->m_dwPid] = pInfo;
	}
}

ProcInfo* DataCenter::GetProcInfo(DWORD dwPid)
{
	ProcInfo* pInfo = NULL;
	if (m_mapPid2ProcInfos.find(dwPid) != m_mapPid2ProcInfos.end())
	{
		pInfo = m_mapPid2ProcInfos[dwPid];
	}

	return pInfo;
}

void DataCenter::ClearProcInfos()
{
	std::map< DWORD, ProcInfo* >::iterator iter;
	for (iter = m_mapPid2ProcInfos.begin(); iter != m_mapPid2ProcInfos.end(); iter++)
	{
		ReuseProcInfo(iter->second);
	}

	m_mapPid2ProcInfos.clear();
}

void DataCenter::GetAllProcInfos(std::vector< ProcInfo* > & vecProcInfos)
{
	std::map< DWORD, ProcInfo* >::iterator iter;
	for (iter = m_mapPid2ProcInfos.begin(); iter != m_mapPid2ProcInfos.end(); iter++)
	{
		vecProcInfos.push_back(iter->second);
	}
}

int DataCenter::GetProcCount()
{
	return m_mapPid2ProcInfos.size();
}