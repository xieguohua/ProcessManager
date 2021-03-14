#include "stdafx.h"
#include "UsageDetector.h"

#pragma comment(lib, "pdh.lib")

UsageDetector::UsageDetector() : m_bInitialized(FALSE), m_hCounter(NULL), m_hQuery(NULL)
{

}

UsageDetector::~UsageDetector()
{

}

HRESULT UsageDetector::Initialize()
{
    if (m_bInitialized)
        return S_OK;

    PDH_STATUS status = PdhOpenQuery(NULL, NULL, &m_hQuery);
    if (ERROR_SUCCESS != status)
        return E_FAIL;

    status = PdhAddCounter(m_hQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &m_hCounter);
    if (ERROR_SUCCESS != status)
        return E_FAIL;

    m_bInitialized = TRUE;
    return S_OK;
}

HRESULT UsageDetector::Uninitialize()
{
    if (!m_bInitialized)
        return E_FAIL;

    PDH_STATUS status = PdhCloseQuery(m_hQuery);
    if (ERROR_SUCCESS != status)
        return E_FAIL;

    m_hCounter = NULL;
    m_hQuery = NULL;
    m_bInitialized = FALSE;
    return S_OK;
}

int UsageDetector::GetCpuUsage()
{
    if (!m_bInitialized)
        return 0;

    PDH_FMT_COUNTERVALUE counterVal;
    PDH_STATUS status = PdhCollectQueryData(m_hQuery);
    if (ERROR_SUCCESS != status)
        return 0;

    status = PdhGetFormattedCounterValue(m_hCounter, PDH_FMT_DOUBLE, NULL, &counterVal);
    if (ERROR_SUCCESS != status)
        return 0;

    return static_cast<int>(counterVal.doubleValue);
}

int UsageDetector::GetMemoryUsage()
{
    if (!m_bInitialized)
        return 0;

    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    BOOL bRet = ::GlobalMemoryStatusEx(&statex);
    if (!bRet)
        return 0;

    return static_cast<int>(statex.dwMemoryLoad);
}

