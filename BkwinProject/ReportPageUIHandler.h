/*************************************************
//  File name:       ReportPageUIHandler.h
//  Date:              2021/01/16
//  <author>      hexujun@cmcm.com
//  <time>          2021/01/16
//   <version>     1.0    
*************************************************/
#ifndef _ReportPageUIHandler_h_
#define _ReportPageUIHandler_h_

#include "bkwin\bkedit\IBkTimer.h"
#include "bkwin\bkownereventdlg.h"
#include "bkwin\IBkDialogWndContext.h"

class ReportPageUIHandler : public BkTimer::IBkTimerNotify
	, public CBkOwnerEventDlg::IChildCtrlEventListener
{
public:
	ReportPageUIHandler(BkWin::IBkDialogWndContext& wndContext) : m_wndContext(wndContext) 
	{
		m_pRootEvnentDlg = NULL;
	}
	~ReportPageUIHandler() {
	}

	enum {
		IDC_TEST = 10,
	};
	enum {
		TIMER_ID_TEST = 10,
		TIMER_ID_TEST_ELAPSE = 30,
	};

	void Initialize(CBkOwnerEventDlg* pOwnerEventDlg) {
		m_pRootEvnentDlg = pOwnerEventDlg;
		if(m_pRootEvnentDlg) {
			m_pRootEvnentDlg->SetListener(this);
		}
		m_wndContext.GetBkTimer().SetTimer(this, TIMER_ID_TEST, TIMER_ID_TEST_ELAPSE);

		BkWin::BkLayoutLoader layout(IDR_LAYOUTTEST);
		m_pRootEvnentDlg->AppendChildEx(layout.GetRootTiXmlElement());//Ìí¼Ó×Ó¿Ø¼þ
	}

	void Uninitialize() {
		if (m_pRootEvnentDlg) {
			m_pRootEvnentDlg->SetListener(NULL);
		}
		m_wndContext.GetBkTimer().KillAlllTimer(this);
		m_wndContext.GetBkRunnableContext().CancelTargetRunnables(this);
	}

protected:
	// for CBkOwnerEventDlg::IChildCtrlEventListener
	virtual void OnChildEventLButtonUp(CBkOwnerEventDlg& ownerDlg, CBkWindow& child, const CPoint& point) {
		if(child.GetCmdID() == IDC_TEST) {
			BkSharePtr<CString> strParam = BkSharePtr<CString>::createObject();
			m_wndContext.GetBkRunnableContext().PostRunnable(
				BkWin::FuncFactory::create_class_func(this, &ReportPageUIHandler::DelayDoSomeThing, strParam), this, 100);
		}
	}

	// for BkTimer::IBkTimerNotify
	virtual void OnBkTimer(UINT uItmerID)  {
	}

protected:
	void DelayDoSomeThing(BkSharePtr<CString> strParam) {
		//do some thing
	}

protected:
	BkWin::IBkDialogWndContext& m_wndContext;
	CBkOwnerEventDlg* m_pRootEvnentDlg;
};

#endif