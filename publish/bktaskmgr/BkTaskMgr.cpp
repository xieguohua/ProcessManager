#include "stdafx.h"
#include "BkTaskMgr.h"

namespace BkWin {\

class RunnableTask : public TaskBase 
{
public:
	RunnableTask(const BkSafePtr<BkWin::IRunnable>& runnable) : m_runnable(runnable)
	{
	}
	~RunnableTask(){}

public:
	virtual void Excute() {
		if (m_runnable) {
			m_runnable->Run();
		}
	}

protected:
	BkSafePtr<BkWin::IRunnable> m_runnable;
};	

void TaskMgr::Initialize()
{
	m_bInitialized = TRUE;
	m_bThreadRuning = FALSE;
	m_eventStop.Reset();
}

void TaskMgr::Uninitialize()
{
	m_bInitialized = FALSE;
	StopWorkThread();
}

void TaskMgr::ExcuteTask(const BkSafePtr<TaskBase>& task, LPVOID lpTarget /*= NULL*/)
{
	if(!task || !m_bInitialized) {
		return;
	}

	BkSafePtr<TaskBase> tempTask = task;

	if (lpTarget) {
		tempTask->SetTarget(lpTarget);
	}
	
	tempTask->StartTickCount();
	StartWorkThread();

	KLocker _auto_(m_lock);
	m_listTask.push_back(tempTask);
	m_eventWork.SetEvent();
}

void TaskMgr::ExcuteTask(const BkSafePtr<BkWin::IRunnable>& runnable, UINT uDelayTime /*= 0*/, LPVOID lpTarget /*= NULL*/)
{
	BkSafePtr<TaskBase> task = BkSafePtr<TaskBase>(new RunnableTask(runnable), false);
	task->SetDelayExcuteTime(uDelayTime);
	ExcuteTask(task, lpTarget);
}

void TaskMgr::CancelTask(LPVOID lpTarget)
{
	if(!lpTarget) {
		return;
	}


	// ‘› ±Œ¥ µœ÷
}

void TaskMgr::OnActivate(KActor* pActor)
{
	CoInitialize(NULL);

	if(pActor == &m_theadWork) {
		DoWorkImpl();
	}

	CoUninitialize();
}

void TaskMgr::ClearTask()
{
	KLocker _auto_(m_lock);

	for (std::list<BkSafePtr<TaskBase>>::iterator iter = m_listTask.begin();
		iter != m_listTask.end();
		++iter) 
	{
		BkSafePtr<TaskBase> pTask = *iter;
		if (pTask->IsMustExcute()) {
			pTask->Excute();
		}
	}
	m_listTask.clear();
}

void TaskMgr::DoWorkImpl()
{
	do 
	{
		DWORD dwResult = m_eventWork.WaitTwoEvent(m_eventStop, FALSE, GetSleepTime());
		if (dwResult == KEvent::WAIT_RESULT_FAILURE) {
			break;
		}

		if(dwResult == KEvent::WAIT_RESULT_OBJECT_1) {
			break;
		}
		
		m_eventWork.Reset();
		PickTaskExcuteImpl();
	} while (true);
	
}

DWORD TaskMgr::GetSleepTime()
{
	KLocker _auto_(m_lock);
	return m_listTask.size() > 0 ? WAIT_ELAPSE : -1;
}

BkSafePtr<TaskBase> TaskMgr::PopShouldExcuteTask()
{
	KLocker _auto_(m_lock);
	for (std::list<BkSafePtr<TaskBase>>::iterator iter = m_listTask.begin();
		 iter != m_listTask.end();
		 ++iter) 
	{
		BkSafePtr<TaskBase> task = *iter;
		if (task->ShouldExcute()) {
			m_listTask.erase(iter);
			return task;
		}
	}

	return NULL;
}

void TaskMgr::PickTaskExcuteImpl()
{
	while(true) 
	{
		BkSafePtr<TaskBase> task = PopShouldExcuteTask();
		if (!task) {
			break;
		}

		task->Excute();
	}
}

void TaskMgr::StartWorkThread()
{
	KLocker _auto_(m_lockThread);
	if (!m_bThreadRuning) 
	{
		m_bThreadRuning = TRUE;
		m_theadWork.Startup(this);
	}
}

void TaskMgr::StopWorkThread()
{
	m_eventStop.SetEvent();

	{
		KLocker _auto_(m_lockThread);
		m_theadWork.Stop();
		m_bThreadRuning = TRUE;
	}
	
	ClearTask();
}

TaskMgr::TaskMgr()
{
	m_bInitialized = FALSE;
	m_eventWork.Create(NULL, NULL, TRUE, FALSE);
	m_eventStop.Create(NULL, NULL, TRUE, FALSE);
}

TaskMgr::~TaskMgr()
{
	ClearTask();
}

}