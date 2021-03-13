/*************************************************
//  File name:       BkTaskMgr.h
//  Date:              2020/12/03
//  <author>      hexujun@cmcm.com
//  <time>          2020/12/03
//   <version>     1.0    
*************************************************/

#ifndef _bkwin_BkTaskMgr_h_
#define _bkwin_BkTaskMgr_h_
#include "Actor\KActor.h"
#include "BkTaskData.h"
#include <list>
#include "include\framework\KLocker.h"
#include "include\framework\KEvent.h"

namespace BkWin {

	class TaskMgr 
		: public IActorCallBack
		, public ITaskMgr
	{
	public:
		TaskMgr();
		~TaskMgr();

		void Initialize();
		void Uninitialize();

		virtual void ExcuteTask(const BkSafePtr<TaskBase>& task, LPVOID lpTarget = NULL) ;
		virtual void ExcuteTask(const BkSafePtr<BkWin::IRunnable>& runnable, UINT uDelayTime = 0, LPVOID lpTarget = NULL);
		virtual void CancelTask(LPVOID lpTarget);

		enum {
			WAIT_ELAPSE = 100,
		};

	protected:
		virtual void OnActivate(KActor* pActor);
		void ClearTask();
		void DoWorkImpl();
		DWORD GetSleepTime();

		BkSafePtr<TaskBase> PopShouldExcuteTask();
		void PickTaskExcuteImpl();
		void StartWorkThread();
		void StopWorkThread();

	protected:
		BOOL m_bThreadRuning;
		BOOL m_bInitialized;
		KActor m_theadWork;
		KCriticalSesion m_lock;
		KCriticalSesion m_lockThread;
		std::list<BkSafePtr<TaskBase>> m_listTask;
		KEvent m_eventWork;
		KEvent m_eventStop;
	};

}

#endif