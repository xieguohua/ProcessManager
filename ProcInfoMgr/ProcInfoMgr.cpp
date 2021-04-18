#include "stdafx.h"
#include "ProcInfoMgr.h"
#include "ProcInfo.h"
#include "ProcInfoDetector.h"

#define PROC_EXPORT_SECTION_NAME _T("process_info")

static std::map<DWORD, ProcInfo*> g_mapPid2Info;

void RemoveOldInfo(DWORD* lpidArr, DWORD dwProcCount)
{
	std::vector<DWORD> vecOldPid;
	std::vector<DWORD>::iterator vecIter;
	std::map<DWORD, ProcInfo*>::iterator iter;
	for (iter = g_mapPid2Info.begin(); iter != g_mapPid2Info.end(); iter++)
	{
		BOOL bExist = FALSE;
		for (int i = 0; i < dwProcCount; i++)
		{
			if (iter->first == lpidArr[i])
			{
				bExist = TRUE;
				break;
			}
		}

		if (bExist == FALSE)
		{
			vecOldPid.push_back(iter->first);
		}
	}

	for (vecIter = vecOldPid.begin(); vecIter != vecOldPid.end(); vecIter++)
	{
		g_mapPid2Info.erase(*vecIter);
	}
}

ProcInfo* GetProcInfo(DWORD dwPid)
{
	ProcInfo* pInfo = NULL;
	ProcInfo* pExistInfo = NULL;
	HANDLE hProcess = NULL;
	HMODULE hModule;
	DWORD dwNeed;
	DWORD dwTime = ::GetTickCount();

	if (g_mapPid2Info.find(dwPid) != g_mapPid2Info.end())
	{
		pExistInfo = g_mapPid2Info[dwPid];
		if (dwTime - g_mapPid2Info[dwPid]->m_dwUpdateTime < 500)
		{
			pInfo = pExistInfo;
			goto Exit0;
		}
	}

    hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);
	if (hProcess == NULL || hProcess == INVALID_HANDLE_VALUE)
	{
		TCHAR chBuf[1024];
		wsprintf(chBuf, __TEXT("OpenProcess %d failed (%d)./n"), dwPid, GetLastError());
		OutputDebugString(chBuf);
		goto Exit0;
	}

	if ( EnumProcessModules( hProcess, &hModule, sizeof(hModule), &dwNeed) == FALSE)
	{
		TCHAR chBuf[1024];
		wsprintf(chBuf, __TEXT("EnumProcessModules %d failed (%d)./n"), dwPid, GetLastError());
		OutputDebugString(chBuf);
		goto Exit0;
	}

    ProcInfoDetector* detector = ProcInfoDetector::GetInstance();
	if (pExistInfo == NULL)
	{
		pExistInfo = new ProcInfo();
		pExistInfo->m_dwPid = dwPid;
		g_mapPid2Info[dwPid] = pExistInfo;

        // 不会更新的数据
        // process name
        TCHAR szProcessName[MAX_PATH] = {0};
        ::GetModuleBaseName(hProcess, hModule, szProcessName, sizeof(szProcessName));
        pExistInfo->m_strProcName = szProcessName;

        // file path
        TCHAR szFilePath[MAX_PATH] = {0};
        ::GetModuleFileNameEx(hProcess, hModule, szFilePath, sizeof(szFilePath));
        pExistInfo->m_strProcPath = szFilePath;

        pExistInfo->m_strCreateTime = detector->GetProcessCreateTime(hProcess);
        pExistInfo->m_dwParentPid = detector->GetProcessParentPID(hProcess);
        pExistInfo->m_dwSessionId = detector->GetProcessSessionID(hProcess);
	}

	pInfo = pExistInfo;
	pInfo->m_dwUpdateTime = dwTime;
	{
        //以下是会实时更新的数据
		// memory usage
		PROCESS_MEMORY_COUNTERS processMemCounters = {0};
		processMemCounters.cb = sizeof(PROCESS_MEMORY_COUNTERS);
        BOOL bRet = ::GetProcessMemoryInfo(hProcess, &processMemCounters, sizeof(PROCESS_MEMORY_COUNTERS));
        pInfo->m_dwMemoryUse = bRet ? (DWORD)(processMemCounters.WorkingSetSize / 1024) : 0;
        
        // cpu usage
        pInfo->m_dwCpuUse = detector->GetProcessCpuUsage(pInfo->m_strProcName);
        pInfo->m_dwHandleCount = detector->GetProcessHandleCount(hProcess);
        pInfo->m_dwUserObjectCount = ::GetGuiResources(hProcess, GR_USEROBJECTS);
        pInfo->m_dwGDIObjectCount = ::GetGuiResources(hProcess, GR_GDIOBJECTS);
	}

Exit0:
    if (NULL != hProcess)
    {
        CloseHandle(hProcess);
    }

	return pInfo;
}

PROCINFOMGR_API HRESULT GetProcInfoMgrVer()
{
	return 1;
}

PROCINFOMGR_API HRESULT GetProcIds(DWORD* lpidArr, DWORD dwBuffSize, DWORD* pDwProcCount)
{
	DWORD dwNeeded;
	if (EnumProcesses(lpidArr, sizeof(DWORD) * dwBuffSize, &dwNeeded))
	{
		if (pDwProcCount != NULL)
		{
			*pDwProcCount = dwNeeded / sizeof(DWORD);
		}

		RemoveOldInfo(lpidArr,  *pDwProcCount);
	}
	else
	{
		if (pDwProcCount != NULL)
		{
			*pDwProcCount = -1;
		}

		TCHAR chBuf[1024];
		wsprintf(chBuf, __TEXT("EnumProcesses failed (%d)./n"), GetLastError());
		OutputDebugString(chBuf);
	}

	return S_OK;
}

PROCINFOMGR_API HRESULT GetProcBaseInfo(DWORD dwPid, GetProcBaseInfoCallback pCbFunc)
{
	ProcInfo* pInfo = GetProcInfo(dwPid);
	if (pInfo != NULL && pCbFunc != NULL)
	{
		pCbFunc(pInfo);
	}

	return S_OK;
}

PROCINFOMGR_API HRESULT CloseProc(DWORD dwPid)
{
	UINT unExitCode = 0;
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (hProc != INVALID_HANDLE_VALUE)
	{
		TerminateProcess(hProc, unExitCode);
	}

	return S_OK;
}

PROCINFOMGR_API HRESULT ExportProcInfo(DWORD dwPid, LPCTSTR lpszExportPath)
{
	ProcInfo* pInfo = GetProcInfo(dwPid);
	if (pInfo != NULL)
	{
		CString strPid;
        strPid.Format(_T("pid:%d"), pInfo->m_dwPid);

		CString strCpuUse;
		strCpuUse.Format(_T("%d"), pInfo->m_dwCpuUse);

		CString strMemoryUse;
		strMemoryUse.Format(_T("%d"), pInfo->m_dwMemoryUse);
		::WritePrivateProfileString(strPid, _T("proc_name"), pInfo->m_strProcName, lpszExportPath);
		::WritePrivateProfileString(strPid, _T("proc_path"), pInfo->m_strProcPath, lpszExportPath);
		::WritePrivateProfileString(strPid, _T("cpu_use"), strCpuUse, lpszExportPath);
		::WritePrivateProfileString(strPid, _T("memory_use"), strMemoryUse, lpszExportPath);
	}

	return S_OK;
}

PROCINFOMGR_API HRESULT ExportAllProcInfo(LPCTSTR lpszExportPath)
{
    std::map<DWORD, ProcInfo*>::iterator iter;
    for (iter = g_mapPid2Info.begin(); iter != g_mapPid2Info.end(); iter++)
    {
        ExportProcInfo(iter->first, lpszExportPath);
    }

    return S_OK;
}

// PROCINFOMGR_API HRESULT ShowLocalFile(DWORD nPid)
// {
// 	return S_OK;
// }
// 
// PROCINFOMGR_API HRESULT ShowProperty(DWORD nPid)
// {
// 	return S_OK;
// }
