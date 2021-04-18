#include "ProcInfoDetector.h"
#include "Utils.h"
#include <Pdh.h>

#pragma comment(lib, "pdh.lib")

ProcInfoDetector* ProcInfoDetector::GetInstance()
{
    static ProcInfoDetector detector;
    return &detector;
}

ProcInfoDetector::ProcInfoDetector()
{
    m_hNtdllModule = ::LoadLibrary(L"ntdll.dll");
    if (NULL == m_hNtdllModule)
        return;

    m_NtQueryInformationProcess = (NtQueryInformationProcessType)::GetProcAddress(m_hNtdllModule, "NtQueryInformationProcess");
}

ProcInfoDetector::~ProcInfoDetector()
{
    if (NULL != m_hNtdllModule)
    {
        ::FreeLibrary(m_hNtdllModule);
        m_hNtdllModule = NULL;
    }
}

ProcInfoDetector::ProcInfoDetector(const ProcInfoDetector& obj)
{

}

ProcInfoDetector& ProcInfoDetector::operator=(const ProcInfoDetector& obj)
{
    return *this;
}

DWORD ProcInfoDetector::GetProcessParentPID(HANDLE hProcess)
{
    DWORD dwFunRet = 0;
    LONG st = -1;
    PROCESS_BASIC_INFORMATION BasicInfo = {0};
    DWORD dwRetSize = 0;

    try
    {
        st = m_NtQueryInformationProcess(hProcess, ProcessBasicInformation, &BasicInfo, sizeof(BasicInfo), &dwRetSize);
        if (st < 0)
            goto Exit0;

        if (BasicInfo.InheritedFromUniqueProcessId == 0)
            goto Exit0;

        dwFunRet = BasicInfo.InheritedFromUniqueProcessId;
    }
    catch (...) {}

Exit0:
    return dwFunRet;
}

DWORD ProcInfoDetector::GetProcessHandleCount(HANDLE hProcess)
{
    LONG st = -1;
    DWORD dwFunRet = 0;
    DWORD dwRetSize = 0;
    DWORD dwHandleCount = 0;

    try
    {
        st = m_NtQueryInformationProcess(hProcess, ProcessHandleCount, &dwHandleCount, sizeof(DWORD), &dwRetSize);
        if (st < 0)
            goto Exit0;

        dwFunRet = dwHandleCount;
    }
    catch (...) {}

Exit0:
    return dwFunRet;
}

CString ProcInfoDetector::GetProcessCreateTime(HANDLE hProcess)
{
    FILETIME ftCreationTime = {0};
    FILETIME ftExitTime = {0};
    FILETIME ftKernelTime = {0};
    FILETIME ftUserTime = {0};
    BOOL bRet = ::GetProcessTimes(hProcess, &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime);
    CString strCreateTime;
    if (bRet)
    {
        ULONGLONG uLongLongTime = Utils::FileTimeToULONGLONG(ftCreationTime);
        uLongLongTime += Utils::GetTimezoneNanoSeconds();
        ftCreationTime = Utils::ULONGLONGToFileTime(uLongLongTime);

        SYSTEMTIME systemTime;
        ::FileTimeToSystemTime(&ftCreationTime, &systemTime);
        strCreateTime.Format(L"%02d:%02d:%02d", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
    }
    return strCreateTime;
}

DWORD ProcInfoDetector::GetProcessSessionID(HANDLE hProcess)
{
    DWORD dwFunRet = 0;
    LONG st = -1;
    PROCESS_SESSION_INFORMATION psi = {0};
    DWORD dwRetSize = 0;

    try
    {
        st = m_NtQueryInformationProcess(hProcess, ProcessSessionInformation, &psi, sizeof(psi), &dwRetSize);
        if (st < 0)
            goto Exit0;

        dwFunRet = psi.SessionId;
    }
    catch (...) {}

Exit0:
    return dwFunRet;
}

int ProcInfoDetector::GetProcessCpuUsage(const CString& strProcessName)
{
    int nRet = 0;
    HQUERY hQuery;
    HCOUNTER hCounter;
    PDH_STATUS status = PdhOpenQuery(NULL, NULL, &hQuery);
    if (ERROR_SUCCESS != status)
        return 0;

    PDH_FMT_COUNTERVALUE counterVal;
    CString strCounterPath;
    strCounterPath.Format(L"\\Processor(%s)\\%% Processor Time", strProcessName);
    status = PdhAddCounter(hQuery, strCounterPath, NULL, &hCounter);
    if (ERROR_SUCCESS != status)
        goto Exit0;

    status = PdhCollectQueryData(hQuery);
    if (ERROR_SUCCESS != status)
        goto Exit0;

    status = PdhGetFormattedCounterValue(hCounter, PDH_FMT_DOUBLE, NULL, &counterVal);
    if (ERROR_SUCCESS != status)
        goto Exit0;

    nRet = static_cast<int>(counterVal.doubleValue);
Exit0:
    PdhCloseQuery(hQuery);
    return nRet;
}

