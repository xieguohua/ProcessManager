/*************************************************
//  File name:       BkTaskData.h
//  Date:              2020/12/03
//  <author>      hexujun@cmcm.com
//  <time>          2020/12/03
//   <version>     1.0    
*************************************************/
#ifndef _BkWin_BkTaskData_h_
#define _BkWin_BkTaskData_h_
#include "bkwin\bkrunnable.h"

namespace BkWin {

	class TaskBase;

	class ITaskMgr {
	public:
		virtual void ExcuteTask(const BkSafePtr<TaskBase>& task, LPVOID lpTarget = NULL) = 0;
		virtual void ExcuteTask(const BkSafePtr<BkWin::IRunnable>& runnable, UINT uDelayTime = 0, LPVOID lpTarget = NULL) = 0;
		virtual void CancelTask(LPVOID lpTarget) = 0;
	};

	class TaskBase : public BkRefImpl
	{
	public:
		TaskBase();
		virtual ~TaskBase();

		void StartTickCount();
		virtual void Excute() = 0;
		virtual bool ShouldExcute();
		void SetMustExcute(bool bMustExcute);
		bool IsMustExcute() const;
		TaskBase* SetDelayExcuteTime(UINT uDelayTime);

		void SetTarget(LPVOID lpTarget);
		LPVOID GetTarget() const;

	protected:
		UINT m_uDelayExcute;
		DWORD m_dwStartTickCount;
		BOOL m_bMustExcute;

		LPVOID m_lpTarget;
	};

}

#endif