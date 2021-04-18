#pragma once
#include "stdafx.h"
#include <windef.h>

#ifndef PROCESSINFOCLASS
typedef enum _PROCESSINFOCLASS 
{
    ProcessBasicInformation,
    ProcessQuotaLimits,
    ProcessIoCounters,
    ProcessVmCounters,
    ProcessTimes,
    ProcessBasePriority,
    ProcessRaisePriority,
    ProcessDebugPort,
    ProcessExceptionPort,
    ProcessAccessToken,
    ProcessLdtInformation,
    ProcessLdtSize,
    ProcessDefaultHardErrorMode,
    ProcessIoPortHandlers,          // Note: this is kernel mode only
    ProcessPooledUsageAndLimits,
    ProcessWorkingSetWatch,
    ProcessUserModeIOPL,
    ProcessEnableAlignmentFaultFixup,
    ProcessPriorityClass,
    ProcessWx86Information,
    ProcessHandleCount,
    ProcessAffinityMask,
    ProcessPriorityBoost,
    ProcessDeviceMap,
    ProcessSessionInformation,
    ProcessForegroundInformation,
    ProcessWow64Information,
    MaxProcessInfoClass
}PROCESSINFOCLASS;
#endif

typedef struct _PROCESS_SESSION_INFORMATION 
{
    ULONG SessionId;	// 进程的 SessionID
} PROCESS_SESSION_INFORMATION, *PPROCESS_SESSION_INFORMATION;

// 进程的基本信息结构体，主要供 NtQueryInformationProcess 使用
typedef struct _PROCESS_BASIC_INFORMATION 
{ 
    LONG ExitStatus;					// 进程终止状态 
    ULONG PebBaseAddress;				// 进程环境块地址
    ULONG AffinityMask; 				// 进程关联掩码
    ULONG BasePriority; 				// 进程的优先级
    ULONG UniqueProcessId; 				// 进程ID
    ULONG InheritedFromUniqueProcessId;	// 父进程ID 
}PROCESS_BASIC_INFORMATION;

typedef LONG (CALLBACK* NtQueryInformationProcessType)(
    IN HANDLE ProcessHandle,
    IN PROCESSINFOCLASS ProcessInformationClass,
    OUT PVOID ProcessInformation,
    IN ULONG ProcessInformationLength,
    OUT PULONG ReturnLength);

class ProcInfoDetector
{
public:
    static ProcInfoDetector* GetInstance();
    DWORD GetProcessParentPID(HANDLE hProcess);
    DWORD GetProcessHandleCount(HANDLE hProcess);
    DWORD GetProcessSessionID(HANDLE hProcess);
    CString GetProcessCreateTime(HANDLE hProcess);
    int GetProcessCpuUsage(const CString& strProcessName);

private:
    ProcInfoDetector();
    ~ProcInfoDetector();
    ProcInfoDetector(const ProcInfoDetector& obj);
    ProcInfoDetector& operator=(const ProcInfoDetector& obj);

private:
    HMODULE m_hNtdllModule;
    NtQueryInformationProcessType m_NtQueryInformationProcess;
};