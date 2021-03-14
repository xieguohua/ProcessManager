#pragma once
#include <Pdh.h>

class UsageDetector
{
public:
    UsageDetector();
    ~UsageDetector();
    HRESULT Initialize();
    HRESULT Uninitialize();
    int GetCpuUsage();
    int GetMemoryUsage();

private:
    BOOL m_bInitialized;
    HQUERY m_hQuery;
    HCOUNTER m_hCounter;
};