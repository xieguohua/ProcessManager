//********************************************************************
//author:  hexujun@cmcm.com
//create time: 2020/6/22?13:07
//description: √Ë ˆ
//*********************************************************************

#ifndef _H_BKSYNCSTATUSDLGBTN_H_
#define _H_BKSYNCSTATUSDLGBTN_H_

#include "bkwnddlg.h"
#include "bkchildviewimpl.h"

class CBkSyncStatusDlgBtn
	: public CBkDialogButton
    , public BkChildViewImpl<CBkSyncStatusDlgBtn>
{
public:
	BKOBJ_DECLARE_CLASS_NAME(CBkSyncStatusDlgBtn, "syncstatusbtn")

	CBkSyncStatusDlgBtn() {
		m_bHeadAcceptLBtnDownMsg = TRUE;
	}

public:
	// Modify BkWindow state
	virtual DWORD ModifyState(DWORD dwStateAdd, DWORD dwStateRemove)
	{
		ModifyChildsStatus(dwStateAdd, dwStateRemove);

		return __super::ModifyState(dwStateAdd, dwStateRemove);
	}

	virtual BOOL NeedRedrawWhenStateChange() 
	{
		return TRUE;
	}

	void _ModifyWndState(CBkWindow *pWnd, DWORD dwStateAdd, DWORD dwStateRemove)
	{
		pWnd->ModifyState(dwStateAdd, dwStateRemove);
		if (pWnd->NeedRedrawWhenStateChange())
		{
			pWnd->NotifyInvalidate();
		}
	}

	void _InvalidateControl(CBkWindow *pWnd, BOOL bCheckVisible = TRUE)
	{
		//pWnd->NotifyInvalidate();
		BKINMINVALIDATERECT nms;

		nms.hdr.code        = BKINM_INVALIDATERECT;
		nms.hdr.hwndFrom    = NULL;
		nms.hdr.idFrom      = m_uCmdID;
		nms.hbkWnd          = m_hBkWnd;
		nms.rect            = m_rcWindow;

		::SendMessage(m_hWndContainer, WM_NOTIFY, (LPARAM)nms.hdr.idFrom, (WPARAM)&nms);
	}
protected:

	void ModifyChildsStatus(DWORD dwStateAdd, DWORD dwStateRemove) 
	{
		POSITION pos = m_lstWndChild.GetHeadPosition();
		while (pos != NULL)
		{
			CBkWindow *pBkWndChild = m_lstWndChild.GetNext(pos);
			pBkWndChild->ModifyState(dwStateAdd, dwStateRemove);
		}
	}



};

#endif