/*************************************************
//  File name:       BkDialogViewExtend.h
//  Date:              2020/11/12
//  <author>      hexujun@cmcm.com
//  <time>          2020/11/12
//   <version>     1.0    
*************************************************/
#ifndef _BKWIN_BKDIALOGVIEWEXTEND_H_
#define _BKWIN_BKDIALOGVIEWEXTEND_H_

#include "BkWndMsgFunction.h"
#include "CBkDialogViewImplEx.h"
#include "bkedit\BkTimerMgr.h"
#include "IBkDialogWndContext.h"

namespace BkWin {\

template<class T>
class BkDialogViewExtend 
	: public CBkDialogViewImplEx<T>
	, public CBkWndMsgFunction<T>
	, public IBkDialogWndContext
 {
 public:
	 
	 BkDialogViewExtend(UINT uResID = 0): CBkDialogViewImplEx<T>(uResID) 
	 {
	 }

	 enum {
		 ID_TIMER_MGR = 0x0FFFF7777,
		 ID_TIMER_MGR_ELAPSE = 5,
	 };

	 BEGIN_MSG_MAP_EX(BkDialogViewExtend<T>)
		 MSG_WM_CREATE(OnCreate)
		 MSG_WM_DESTROY(OnDestroy)
		 MSG_WM_TIMER(OnTimer)
		 CHAIN_MSG_MAP(CBkWndMsgFunction<T>)
		 CHAIN_MSG_MAP(CBkDialogViewImplEx<T>)
	END_MSG_MAP()

public:
	 BkTimer::IBkWndTimer& GetTimerMgr() {
		 return m_timerMgr;
	 }


 private:
	 
	 int OnCreate(LPCREATESTRUCT lpCreateStruct) 
	 {
		 m_timerMgr.Init(m_hWnd, ID_TIMER_MGR, ID_TIMER_MGR_ELAPSE);
		 SetMsgHandled(FALSE);
		 return 0;
	 }

	 void OnDestroy() 
	 {
		 BKDestroy();
		 SetMsgHandled(FALSE);
		 m_timerMgr.UnInit();
	 }

	 void OnTimer(UINT_PTR nIDEvent)
	 {
		 if(nIDEvent == ID_TIMER_MGR) {
			 m_timerMgr.TriggerTimerEvent((UINT)nIDEvent);
		 }
		 else {
			 SetMsgHandled(FALSE);
		 }
	 }

 protected:
	 virtual BkTimer::IBkWndTimer& GetBkTimer() {
		 return GetTimerMgr();
	 }
	 virtual IBkRunnableContext& GetBkRunnableContext()  {
		 return *this;
	 }

 protected:
	 BkTimer::CBkWndTimerMgr m_timerMgr;
};


}
#endif