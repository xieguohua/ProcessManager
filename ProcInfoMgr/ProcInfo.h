#pragma once

class ProcInfo
{
public:
	ProcInfo() {}
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