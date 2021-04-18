#pragma once

class ProcInfo
{
public:
	ProcInfo() 
        : m_dwPid(0)
        , m_dwParentPid(0)
        , m_dwSessionId(0)
		, m_dwCpuUse(0)
		, m_dwMemoryUse(0)
		, m_dwUpdateTime(0)
        , m_dwHandleCount(0)
        , m_dwUserObjectCount(0)
        , m_dwGDIObjectCount(0)
	{
	}

	~ProcInfo() {}

public:
	DWORD m_dwPid;
    DWORD m_dwParentPid;
	DWORD m_dwCpuUse;
	DWORD m_dwMemoryUse;
	DWORD m_dwUpdateTime;
    DWORD m_dwHandleCount;
    DWORD m_dwSessionId;
    DWORD m_dwUserObjectCount;
    DWORD m_dwGDIObjectCount;
    CString m_strProcName;
    CString m_strProcPath;
    CString m_strCreateTime;
};