/*
 * 创建者:      guanjianchang@cmcm.com
 * 创建时间:    2021-03-13
 */

class ProcInfo;
#ifdef PROCINFOMGR_EXPORTS
#define PROCINFOMGR_API __declspec(dllexport)
#else
#define PROCINFOMGR_API __declspec(dllimport)
#endif

typedef void (*GetProcBaseInfoCallback)(ProcInfo* pInfo);

PROCINFOMGR_API HRESULT GetProcInfoMgrVer();
PROCINFOMGR_API HRESULT GetProcIds(DWORD* lpidArr, DWORD dwBuffSize, DWORD* pDwProcCount);
PROCINFOMGR_API HRESULT GetProcBaseInfo(DWORD dwPid, GetProcBaseInfoCallback pCbFunc);
PROCINFOMGR_API HRESULT CloseProc(DWORD dwPid);
PROCINFOMGR_API HRESULT ExportProcInfo(DWORD dwPid, LPCTSTR lpszExportPath);
PROCINFOMGR_API HRESULT ExportAllProcInfo(LPCTSTR lpszExportPath);
// PROCINFOMGR_API HRESULT ShowLocalFile(DWORD dwPid);
// PROCINFOMGR_API HRESULT ShowProperty(DWORD dwPid);