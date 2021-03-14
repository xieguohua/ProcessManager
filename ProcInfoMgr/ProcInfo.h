#pragma once

class ProcInfo
{
public:
	ProcInfo() 
		: m_dwPid(0)
		, m_dwCpuUse(0)
		, m_dwMemoryUse(0)
		, m_dwUpdateTime(0)
	{
	}

	~ProcInfo() {}

public:
	DWORD m_dwPid;
	CString m_strProcName;
	CString m_strProcPath;
	DWORD m_dwCpuUse;
	DWORD m_dwMemoryUse;

	// TODO

	DWORD m_dwUpdateTime;
};