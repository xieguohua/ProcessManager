#include "KActor.h"
#include <process.h>
#include <atlbase.h>
#include <atlapp.h>
//#include "include/framework/SetThreadName.h"

#define	WAIT_THREAD_TIMEOUT				(200LL)


KActor::KActor()
{
	m_uThreadID			= 0L;
	m_pThread			= NULL;
	m_hStopEvent		= NULL;
	m_hWaitEvent		= NULL;
	m_pCallBack			= NULL;
	m_bWinInit			= false;
	m_nParam = 0;
}

KActor::~KActor()
{
	Stop();
}
void KActor::SetParam(int nParam)
{
    m_nParam = nParam;
}
void KActor::GetParam(int& nParam)
{
    nParam = m_nParam;
}
HRESULT KActor::Startup(const char* strThreadName, IActorCallBack* pCallBack, bool bWinInit /* = false */)
{
	HRESULT lResult		= E_FAIL;
	HRESULT lRetCode	= E_FAIL;


	DeActivate();


	m_pCallBack = pCallBack;
	m_bWinInit = bWinInit;


	lRetCode = Activate(strThreadName);


	lResult = S_OK;

Exit0:

	if (FAILED(lResult))
	{
		Stop();
	}

	return lResult;
}

HRESULT KActor::Stop()
{
	return DeActivate();
}

bool KActor::IsActive()
{
	return (m_uThreadID == GetCurrentThreadId());
}

bool KActor::IsThreadRunning()
{
    bool bRet = FALSE;
    if ( m_pThread )
    {
        DWORD dwWait = WaitForSingleObject(m_pThread, 0);
        if ( dwWait == WAIT_TIMEOUT )
        {
            bRet = TRUE;
        }
    }

    return bRet;
}

bool KActor::WaitForStop(DWORD dwMilliseconds)
{
	HANDLE hEvents[2] = {m_hStopEvent, m_hWaitEvent}; 


	::ResetEvent(m_hWaitEvent);

	DWORD dwRet = WaitForMultipleObjects(
		2,
		hEvents,
		FALSE,		// wait for any
		dwMilliseconds);
	if (WAIT_OBJECT_0 + 0 == dwRet)	// stop event was signaled.
		return true;	

	return false;	
}

#define PER_SAMPLE_TIME		1000

bool KActor::WaitForCPUFree(DWORD dwSampleSeconds /* = 10 */, int nCPULowUsage /* = 50 */)
{
	typedef BOOL WINAPI GetSystemTimes(
		__out_opt  LPFILETIME lpIdleTime,
		__out_opt  LPFILETIME lpKernelTime,
		__out_opt  LPFILETIME lpUserTime);

	GetSystemTimes* GetSystemTimesFn = NULL;
	GetSystemTimesFn = (GetSystemTimes*)GetProcAddress(GetModuleHandle(L"kernel32"), "GetSystemTimes");
	if (!GetSystemTimesFn)
		return true;


	UINT			nLoop				= 0;
	UINT			nCpuUsageSum		= 0;
	__int64			nLastIdleTime		= 0;
	__int64			nLastKernelTime		= 0;
	__int64			nLastUserTime		= 0;

	do 
	{
		__int64			nNowIdleTime	= 0;
		__int64			nNowKernelTime	= 0;
		__int64			nNowUserTime	= 0;

		if (!GetSystemTimesFn((LPFILETIME)&nNowIdleTime, (LPFILETIME)&nNowKernelTime, (LPFILETIME)&nNowUserTime))
			return true;

		// if it's a first call - skip it
		if (nLastIdleTime != 0)
		{
			__int64 nIdleTime = nNowIdleTime - nLastIdleTime;
			__int64 nSystemTime = nNowKernelTime + nNowUserTime - nLastKernelTime - nLastUserTime;

			nCpuUsageSum += (UINT)(100.0 - (double)nIdleTime / (double)nSystemTime * 100.0);
			nLoop++;

			if (nLoop >= (dwSampleSeconds * 1000 / PER_SAMPLE_TIME))
			{
				if ((nCpuUsageSum / nLoop) <= (UINT)nCPULowUsage)
					return true;

				if (WaitForStop(dwSampleSeconds))
					return false;

				nLoop = 0;
				nCpuUsageSum = 0;
			}
		}

		// store new CPU's idle and system time
		nLastIdleTime = nNowIdleTime;
		nLastKernelTime = nNowKernelTime;
		nLastUserTime = nNowUserTime;


		if (WaitForStop(PER_SAMPLE_TIME))
			return false;

	} while (true);	
}

void KActor::StopWait()
{
	if (m_hWaitEvent)
	{
		::SetEvent(m_hWaitEvent);
	}
}

HRESULT KActor::Activate(const char* strThreadName)
{
	HRESULT lResult	= E_FAIL;


	m_hStopEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hWaitEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

	m_pThread = (HANDLE)::_beginthreadex(NULL, 0, OnActivate, this, 0, &m_uThreadID);



	lResult = S_OK;

Exit0:

	return lResult;
}

HRESULT KActor::DeActivate()
{
	HRESULT lResult	= E_FAIL;


	if (m_hStopEvent)
	{
		::SetEvent(m_hStopEvent);
	}


	while (m_pThread)
	{
		if (m_bWinInit)
		{
			PostThreadMessage(m_uThreadID, WM_QUIT, 0, 0);
		}

		DWORD dwWaitCode = ::WaitForSingleObject(m_pThread, WAIT_THREAD_TIMEOUT);
		if (dwWaitCode == WAIT_TIMEOUT)
		{
			continue;
		}

		CloseHandle(m_pThread);
		m_pThread = NULL;
	}


	m_uThreadID	= 0L;
	if (NULL != m_hStopEvent)
	{
		::CloseHandle(m_hStopEvent);
		m_hStopEvent = NULL;
	}

	if (NULL != m_hWaitEvent)
	{
		::CloseHandle(m_hWaitEvent);
		m_hWaitEvent = NULL;
	}


	lResult = S_OK;

Exit0:

	return lResult;
}

unsigned __stdcall KActor::OnActivate(void* lParam)
{
	HRESULT lResult = E_FAIL;
	KActor* pThis	= (KActor*)lParam;


	pThis->m_pCallBack->OnActivate(pThis);

	if (pThis->m_bWinInit)
	{
		CMessageLoop MsgLoop;
		MsgLoop.Run();
	}


	lResult = S_OK;

Exit0:
	return lResult;
}

bool KActor::IsNeedStop() const
{
    return (WAIT_OBJECT_0 == ::WaitForSingleObject(m_hStopEvent, 0));
}
