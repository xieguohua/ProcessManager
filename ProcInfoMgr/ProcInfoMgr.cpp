#include "stdafx.h"
#include "ProcInfoMgr.h"
#include "ProcInfo.h"

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

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (hProcess == INVALID_HANDLE_VALUE)
	{
		TCHAR chBuf[1024];
		wsprintf(chBuf, __TEXT("OpenProcess %d failed (%d)./n"), dwPid, GetLastError());
		OutputDebugString(chBuf);
		goto Exit0;
	}

	if (pExistInfo == NULL)
	{
		pExistInfo = new ProcInfo();
		pExistInfo->m_dwPid = dwPid;
		g_mapPid2Info[dwPid] = pExistInfo;
	}

	pInfo = pExistInfo;
	pInfo->m_dwUpdateTime = dwTime;

	{
		// process name
		TCHAR szProcessName[256];
		GetModuleBaseName(hProcess, NULL, szProcessName, 256);
		pInfo->m_strProcName = szProcessName;

		// file path
		TCHAR szFilePath[256] ;
		GetModuleFileNameEx(hProcess, NULL, szFilePath, 256);
		pInfo->m_strProcPath = szFilePath;

		// memory use
		PROCESS_MEMORY_COUNTERS processMemCounters = {0};
		processMemCounters.cb = sizeof(PROCESS_MEMORY_COUNTERS);
		DWORD bRet = GetProcessMemoryInfo(hProcess, &processMemCounters, sizeof(PROCESS_MEMORY_COUNTERS));
		pInfo->m_dwMemoryUse = (DWORD)(processMemCounters.WorkingSetSize / 1024);
	}

	CloseHandle(hProcess);

Exit0:
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
		pCbFunc(pInfo->m_strProcName, pInfo->m_dwPid, pInfo->m_strProcPath, pInfo->m_dwCpuUse, pInfo->m_dwMemoryUse);
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
		strPid.Format(_T("%d"), pInfo->m_dwPid);

		CString strCpuUse;
		strCpuUse.Format(_T("%d"), pInfo->m_dwCpuUse);

		CString strMemoryUse;
		strMemoryUse.Format(_T("%d"), pInfo->m_dwMemoryUse);

		::WritePrivateProfileString(PROC_EXPORT_SECTION_NAME, _T("pid"), strPid, lpszExportPath);
		::WritePrivateProfileString(PROC_EXPORT_SECTION_NAME, _T("proc_name"), pInfo->m_strProcName, lpszExportPath);
		::WritePrivateProfileString(PROC_EXPORT_SECTION_NAME, _T("proc_path"), pInfo->m_strProcPath, lpszExportPath);
		::WritePrivateProfileString(PROC_EXPORT_SECTION_NAME, _T("cpu_use"), strCpuUse, lpszExportPath);
		::WritePrivateProfileString(PROC_EXPORT_SECTION_NAME, _T("memory_use"), strMemoryUse, lpszExportPath);
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
