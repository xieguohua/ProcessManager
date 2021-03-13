
//********************************************************************
//author:  hexujun@cmcm.com
//create time: 2020/6/19?14:41
//description: √Ë ˆ
//*********************************************************************

#ifndef _H_BKIN_BKWNDMSGFUNCTION_H_
#define _H_BKIN_BKWNDMSGFUNCTION_H_

#include "include\framework\KLocker.h"
#include "IBkRunnableContext.h"
#include <list>
#include <atlwin.h>

namespace BkWin
{

template<class WindowClassT>
class CBkWndMsgFunction : public IBkRunnableContext
{
public:

	typedef DWORD RUNNABLE_ID;

	enum
	{
		INVALIDATE_RUNNABLE_ID = -1,
	};

	CBkWndMsgFunction(UINT uTimerID = 10000001) 
	{
		m_bStartTime = FALSE;
		m_uTimerID = uTimerID;
		m_dwNextRunnbaleID = 0;
	}

	~CBkWndMsgFunction() 
	{
		ClearRunables();
	}

public:
	BEGIN_MSG_MAP_EX(CBkWndMsgFunction)
		MESSAGE_HANDLER_EX(GetPostFunctMsg(), OnMsgPostFunction)
		MESSAGE_HANDLER_EX(GetSendFunctMsg(), OnMsgSendFunction)
		MSG_WM_TIMER(OnTimer)
	END_MSG_MAP()

public:

	class RunnableBase : public BkRefImpl
	{
	public:
		RunnableBase() 
		{
			dwStartTick = 0;
			dwDelayTime = 0;
			dwID = 0;
			bCancled = FALSE;
		}

		virtual ~RunnableBase(){}
		virtual LRESULT Run() = 0;

		bool IsReachTime() const 
		{
			return GetTickCount() - dwStartTick >= dwDelayTime;
		}

		virtual LPVOID GetTarget() const {
			return NULL;
		}

	public:
		DWORD dwStartTick;
		DWORD dwDelayTime;
		DWORD dwID;
		BOOL    bCancled;
	};

	template<class T>
	struct ObjMemberFunction : public RunnableBase
	{
		typedef LRESULT (T::*func_ptr)(WPARAM, LPARAM);

		T* pObject;
		func_ptr pFunc;

		WPARAM wParam;
		LPARAM lParam;

		ObjMemberFunction() 
		{
			wParam = lParam = 0;
			pObject = NULL;
			pFunc = NULL;
		}

		virtual LRESULT Run() 
		{
			if(pObject == NULL || pFunc == NULL) 
			{
				return S_FALSE;
			}

			return (pObject->*pFunc)(wParam, lParam);
		}

		virtual LPVOID GetTarget() const {
			return (LPVOID)pObject;
		}
	};

	class RunnableWrap : public RunnableBase 
	{
	public:
		RunnableWrap(IRunnable* runnable, LPVOID lpTarget = NULL) {
			m_runnable = runnable;
			lpTarget = lpTarget;
		}

		virtual LPVOID GetTarget() const {
			return (LPVOID)lpTarget;
		}

		virtual LRESULT Run() 
		{
			if(!m_runnable) 
			{
				return S_FALSE;
			}

			m_runnable->Run();

			return S_OK;
		}

	protected:
		BkSafePtr<IRunnable> m_runnable;
		LPVOID lpTarget;
	};

public:

	virtual BOOL SendRunnable(BkSafePtr<IRunnable>& runnable) {
		if(!runnable || !((WindowClassT*)this)->IsWindow()) 
		{
			return FALSE;
		}

		RunnableWrap runnableWrap(runnable);
		LRESULT lResult =  ((WindowClassT*)this)->SendMessage(GetSendFunctMsg(), 0, (LPARAM)&runnableWrap);
		return lResult == S_OK;
	}

	virtual DWORD PostRunnable(BkSafePtr<IRunnable>& runnable, LPVOID lpTarget = NULL, UINT uDelayTime = 0) {
		if(!runnable || !((WindowClassT*)this)->IsWindow()) 
		{
			return 0;
		}

		KLocker lock(m_locker);

		BkSafePtr<RunnableWrap> runnableWrap(new RunnableWrap(runnable, lpTarget), false);
		runnableWrap->dwStartTick = ::GetTickCount();
		runnableWrap->dwDelayTime = uDelayTime;
		runnableWrap->dwID = (++m_dwNextRunnbaleID);

		m_setPostRunnables.push_back(runnableWrap.Ptr());
		((WindowClassT*)this)->PostMessage(GetPostFunctMsg(), 0, (LPARAM)runnableWrap.Ptr());

		return runnableWrap->dwID;
	}

	template<class T>
	LRESULT SendMessageFunction(T* pObj, LRESULT (T::*pFunc)(WPARAM, LPARAM), WPARAM wParam = 0, LPARAM lParam = 0) 
	{
		if(pFunc == NULL || pObj == NULL || !((WindowClassT*)this)->IsWindow()) 
		{
			return S_FALSE;
		}

		ObjMemberFunction<T> objMemberFunction;

		objMemberFunction.wParam = wParam;
		objMemberFunction.lParam = lParam;
		objMemberFunction.pFunc = pFunc;
		objMemberFunction.pObject = pObj;

		LRESULT lResult =  ((WindowClassT*)this)->SendMessage(GetSendFunctMsg(), 0, (LPARAM)&objMemberFunction);

		return lResult;
	}

	template<class T>
	DWORD PostMessageFunction(T* pObj, LRESULT (T::*pFunc)(WPARAM, LPARAM), WPARAM wParam = 0, LPARAM lParam = 0, DWORD uDelayTime = 0) 
	{
		if(pFunc == NULL || pObj == NULL || !((WindowClassT*)this)->IsWindow()) 
		{
			return INVALIDATE_RUNNABLE_ID;
		}

		KLocker lock(m_locker);

		BkSafePtr<ObjMemberFunction<T>> pObjMemberFunction(new ObjMemberFunction<T>(), false);

		pObjMemberFunction->wParam = wParam;
		pObjMemberFunction->lParam = lParam;
		pObjMemberFunction->pFunc = pFunc;
		pObjMemberFunction->pObject = pObj;
		pObjMemberFunction->dwStartTick = ::GetTickCount();
		pObjMemberFunction->dwDelayTime = uDelayTime;
		pObjMemberFunction->dwID = (++m_dwNextRunnbaleID);

		m_setPostRunnables.push_back(pObjMemberFunction.Ptr());
		((WindowClassT*)this)->PostMessage(GetPostFunctMsg(), 0, (LPARAM)pObjMemberFunction.Ptr());

		return pObjMemberFunction->dwID;
	}

	virtual void CancelRunnableByID(DWORD dwRunnbaleID) 
	{
		KLocker lock(m_locker);

		for (std::list<BkSafePtr<RunnableBase>>::iterator iter = m_setPostRunnables.begin();
			iter != m_setPostRunnables.end();
			++iter) 
		{
			BkSafePtr<RunnableBase> pObj = *iter;
			if (pObj->dwID == dwRunnbaleID) 
			{
				pObj->bCancled = TRUE;
				m_setPostRunnables.erase(iter);
				break;
			}
		}
	}

	virtual void CancelTargetRunnables(LPVOID lpszTarget) {
		if (lpszTarget == NULL) {
			return;
		}

		KLocker lock(m_locker);

		for (std::list<BkSafePtr<RunnableBase>>::iterator iter = m_setPostRunnables.begin();
			  iter != m_setPostRunnables.end();) 
		{
			BkSafePtr<RunnableBase> pObj = *iter;
			if (pObj->GetTarget() == lpszTarget) 
			{
				pObj->bCancled = TRUE;
				iter = m_setPostRunnables.erase(iter);
				continue;
			}

			++iter;
		}
	}

public:
	static UINT GetPostFunctMsg() 
	{
		static UINT uPostMsg = 0;
		if (uPostMsg == 0) 
		{
			uPostMsg = ::RegisterWindowMessage(_T("POST_{D07BC72F-4EAE-4c53-921B-5B576AD6192E}"));
		}

		return uPostMsg;
	}

	static UINT GetSendFunctMsg()
	{
		static UINT uSendMsg = 0;
		if (uSendMsg == 0) 
		{
			uSendMsg = ::RegisterWindowMessage(_T("SEND_{D07BC72F-4EAE-4c53-921B-5B576AD6192E}"));
		}

		return uSendMsg;
	}

private:

	LRESULT OnMsgPostFunction(UINT uMsg, WPARAM wParam, LPARAM lParam) 
	{
		TriggerRunnables();
		return S_OK;
	}

	LRESULT OnMsgSendFunction(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		RunnableBase* pObject = (RunnableBase*)(lParam);
		if(pObject == NULL) 
		{
			return S_FALSE;
		}

		LRESULT lResult = pObject->Run();;
		return lResult;
	}

	void OnTimer(UINT_PTR nIDEvent) 
	{
		if(m_uTimerID != nIDEvent) 
		{
			SetMsgHandled(FALSE);
			return;
		}

		TriggerRunnables();
	}

	void StartTimer() 
	{
		if (!m_bStartTime) 
		{
			((WindowClassT*)this)->SetTimer(m_uTimerID, 5);
			m_bStartTime = TRUE;
		}

	}

	void StopTimer() 
	{
		if (m_bStartTime) 
		{
			((WindowClassT*)this)->KillTimer(m_uTimerID);
			m_bStartTime = FALSE;
		}
	}

	void ClearRunables() 
	{
		KLocker lock(m_locker);
		m_setPostRunnables.clear();
	}

	void TriggerRunnables() 
	{

		while (true) 
		{
			BkSafePtr<RunnableBase> pObj = GetNextCallFunction();
			if (!pObj) {
				break;
			}

			pObj->Run();
		}

		{
			KLocker lock(m_locker);
			if (m_setPostRunnables.empty()) 
			{
				StopTimer();
			}
			else 
			{
				StartTimer();
			}
		}
	}

	BkSafePtr<RunnableBase> GetNextCallFunction()
	{
		// –ﬁ∏¥ø®À¿µƒŒ Ã‚
		KLocker lock(m_locker);

		for (std::list<BkSafePtr<RunnableBase>>::iterator iter = m_setPostRunnables.begin();
			iter != m_setPostRunnables.end();) 
		{
			BkSafePtr<RunnableBase> object = *iter;
			if (object->bCancled) 
			{
				iter = m_setPostRunnables.erase(iter);
				continue;
			}

			if (object->IsReachTime()) 
			{
				iter = m_setPostRunnables.erase(iter);
				return object;
			}

			++iter;
		}

		return NULL;
	}


protected:
	DWORD m_dwNextRunnbaleID;
	UINT m_uTimerID;
	std::list<BkSafePtr<RunnableBase>> m_setPostRunnables;
	BOOL m_bStartTime;
	KCriticalSesion m_locker;
};

class CBkWndMsgFunctionWindow 
	: public CWindowImpl<CBkWndMsgFunctionWindow>
	, public CBkWndMsgFunction<CBkWndMsgFunctionWindow>
{
public:
	CBkWndMsgFunctionWindow()
	{
	}

	BOOL CreateMessageWindow() 
	{
		if(IsWindow()) 
		{
			return FALSE;
		}

		return NULL != Create(HWND_MESSAGE, NULL, NULL, WS_POPUP);
	}

public:
	BEGIN_MSG_MAP_EX(CBkWndMsgFunctionWindow)
		CHAIN_MSG_MAP(CBkWndMsgFunction<CBkWndMsgFunctionWindow>)
	END_MSG_MAP()
};

}
#endif