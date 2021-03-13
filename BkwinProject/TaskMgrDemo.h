
/*************************************************
//  File name:       TaskMgrDemo.h
//  Date:              2021/01/16
//  <author>      hexujun@cmcm.com
//  <time>          2021/01/16
//   <version>     1.0    
*************************************************/
#ifndef _TaskMgrDemo_h_
#define _TaskMgrDemo_h_
#include "bktaskmgr\BkTaskMgr.h"
class TaskMgrDemo {
public:
	TaskMgrDemo() {
		m_pTaskMgr = new BkWin::TaskMgr();
		m_pTaskMgr->Initialize();
	}

	~TaskMgrDemo() {
		m_pTaskMgr->Uninitialize();
		delete m_pTaskMgr;
	}

	void DoSomeThing() {
		BkSharePtr<CString> param1 = BkSharePtr<CString>::createObject();
		int nSrc = 1000;
		m_pTaskMgr->ExcuteTask(
			BkWin::FuncFactory::create_class_func(this, &TaskMgrDemo::DoSomeThingInTaskTread, param1, nSrc),
			100, this);
	}

protected:
	void DoSomeThingInTaskTread(BkSharePtr<CString> param1, int nSrc) {
		// do some thing
	}

protected:
	BkWin::TaskMgr* m_pTaskMgr;
};
#endif