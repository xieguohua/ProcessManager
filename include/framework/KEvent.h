#ifndef _KEVENT_H_
#define _KEVENT_H_

class KEvent
{
public:
	enum 
	{
		WAIT_RESULT_FAILURE = -2,
		WAIT_RESULT_TIME_OUT = -1,
		WAIT_RESULT_OBJECT_0 = 0,
		WAIT_RESULT_OBJECT_1 = 1,
		WAIT_RESULT_OBJECT_2 = 2,
	};

public:
	KEvent()
	{
		m_hEvent = NULL;
	}

	KEvent(HANDLE hEvent)
	{
		m_hEvent = hEvent;
	}

	virtual ~KEvent()
	{
		Close();
	}

	void operator = (HANDLE hEvent)
	{
		Close();

		m_hEvent = hEvent;
	}

	void operator = (KEvent &event)
	{
		Close();

		m_hEvent = event.Detach();
	}

	HANDLE Detach()
	{
		HANDLE hEvent = m_hEvent;
		m_hEvent = NULL;

		return hEvent;
	}

	HANDLE GetHandle()
	{
		return m_hEvent;
	}

	BOOL Open(LPCTSTR szEventName)
	{
		Close();

		m_hEvent = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, szEventName);

		return (m_hEvent != NULL);
	}

	BOOL Create(LPCTSTR szEventName, BOOL bManaulReset, BOOL bInitStatus)
	{
		Close();

		m_hEvent = CreateEvent(NULL, bManaulReset, bInitStatus, szEventName);

		return (m_hEvent != NULL);
	}

	BOOL Create(SECURITY_ATTRIBUTES *sa, LPCTSTR szEventName, BOOL bManaulReset, BOOL bInitStatus)
	{
		Close();

		m_hEvent = CreateEvent(sa, bManaulReset, bInitStatus, szEventName);

		return (m_hEvent != NULL);
	}

	void Close()
	{
		if (m_hEvent)
		{
			CloseHandle(m_hEvent);
			m_hEvent = NULL;
		}
	}

	BOOL Wait(DWORD dwTime)
	{
		BOOL bReturn = FALSE;

		if (m_hEvent == NULL)
			goto Exit0;

		if (WAIT_OBJECT_0 != WaitForSingleObject(m_hEvent, dwTime))
			goto Exit0;

		bReturn = TRUE;

Exit0:
		return bReturn;
	}

    DWORD WaitEx(DWORD dwTime)
    {
        if (m_hEvent)
        {
            return WaitForSingleObject(m_hEvent, dwTime);
        }
        return WAIT_FAILED;
    }

	int WaitTwoEvent(KEvent &secondEvent, BOOL bWaitAll, DWORD dwTime)
	{
		HANDLE hWait[2] = {m_hEvent, secondEvent.m_hEvent};
		DWORD dwResult = WaitForMultipleObjects(2, hWait, bWaitAll, dwTime);

		if (WAIT_OBJECT_0 == dwResult)
			return WAIT_RESULT_OBJECT_0;
		else if (WAIT_OBJECT_0 + 1 == dwResult)
			return WAIT_RESULT_OBJECT_1;
		else if (WAIT_TIMEOUT == dwResult)
			return WAIT_RESULT_TIME_OUT;

		return WAIT_RESULT_FAILURE; //等待出错 例如句柄值无效
	}

	int WaitThreeEvent(KEvent &secondEvent, KEvent &thirdEvent, BOOL bWaitAll, DWORD dwTime)
	{
		HANDLE hWait[3] = {m_hEvent, secondEvent.m_hEvent, thirdEvent.m_hEvent};
		DWORD dwResult = WaitForMultipleObjects(3, hWait, bWaitAll, dwTime);

		if (WAIT_OBJECT_0 == dwResult)
			return WAIT_RESULT_OBJECT_0;
		else if (WAIT_OBJECT_0 + 1 == dwResult)
			return WAIT_RESULT_OBJECT_1;
		else if (WAIT_OBJECT_0 + 2 == dwResult)
			return WAIT_RESULT_OBJECT_2;
		else if (WAIT_TIMEOUT == dwResult)
			return WAIT_RESULT_TIME_OUT;

		return WAIT_RESULT_FAILURE; //等待出错 例如句柄值无效
	}

	int SetEvent()
	{
		int nReturn = -1;

		if (m_hEvent)
		{
			if (::SetEvent(m_hEvent))
				nReturn = 0;
		}

		return nReturn;
	}

	int Reset()
	{
		int nReturn = -1;

		if (m_hEvent)
		{
			if (::ResetEvent(m_hEvent))
				nReturn = 0;
		}

		return nReturn;
	}

private:
	HANDLE m_hEvent;
};
#endif