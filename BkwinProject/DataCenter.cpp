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
		m_vecProcInfos.push_back(pInfo);
	}
}

void DataCenter::ClearProcInfos()
{
	std::vector< ProcInfo* >::iterator iter;
	for (iter = m_vecProcInfos.begin(); iter != m_vecProcInfos.end(); iter++)
	{
		ReuseProcInfo(*iter);
	}

	m_vecProcInfos.clear();
}

std::vector< ProcInfo* > & DataCenter::GetAllProcInfos()
{
	return m_vecProcInfos;
}