//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KActor.h
//  Creator     :   YangXD(yxdh)
//  Date        :   2010-7-25 17:30:50
//  Comment     :   Interface for the KActor class.
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _K_ACTOR_H_
#define _K_ACTOR_H_
#include <wtypes.h>


class KActor;


class IActorCallBack
{
public:
	virtual void						OnActivate(KActor* pActor) = 0;
};


class KActor
{
public:
	KActor();
	~KActor();

	HRESULT								Startup(const char* strThreadName, IActorCallBack* pCallBack, bool bWinInit = false);
//#ifndef DEBUG_PROJ
	HRESULT								Startup(IActorCallBack* pCallBack, bool bWinInit = false)
	{
		return Startup(NULL, pCallBack, bWinInit);
	}
//#endif
	HRESULT								Stop();

	bool								IsActive();
	bool								WaitForStop(DWORD dwMilliseconds);
	bool								WaitForCPUFree(DWORD dwSampleSeconds = 10, int nCPULowUsage = 30);
    bool                                IsThreadRunning();
	void								StopWait();
    bool                                IsNeedStop() const;
    void                                SetParam(int nParam);
    void                                GetParam(int& nParam);
protected:
	HRESULT								Activate(const char* strThreadName);
	HRESULT								DeActivate();

	static unsigned __stdcall 			OnActivate(void* lParam);


private:
	HANDLE								m_pThread;
	unsigned							m_uThreadID;
	HANDLE								m_hStopEvent;
	HANDLE								m_hWaitEvent;
	bool								m_bWinInit;
    int                                 m_nParam;
	IActorCallBack*						m_pCallBack;
};


#endif	// #ifndef _K_ACTOR_H_