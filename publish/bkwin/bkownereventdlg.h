//********************************************************************
//author:  hexujun@cmcm.com
//create time: 2020/6/9?10:01
//description: distpatch事件控件，支持二级事件dispatch, 能够监听自控件的展示
//*********************************************************************

#ifndef _H_BKOWNEREVENTDLG_H_
#define _H_BKOWNEREVENTDLG_H_

#include "bkchildviewimpl.h"

class CBkOwnerEventDlg 
	: public CBkDialog
	, public BkChildViewImpl<CBkOwnerEventDlg>
{
public:
	BKOBJ_DECLARE_CLASS_NAME(CBkOwnerEventDlg, "ownereventdlg")

	friend BkChildViewImpl;

public:

	class IChildCtrlEventListener 
	{
	public:
		virtual void OnChildEventLButtonDown(CBkOwnerEventDlg& ownerDlg, CBkWindow& child, const CPoint& point) {}
		virtual void OnChildEventLButtonUp(CBkOwnerEventDlg& ownerDlg, CBkWindow& child, const CPoint& point) {}
		virtual void OnChildEventRButtonDown(CBkOwnerEventDlg& ownerDlg, CBkWindow& child, const CPoint& point) {}
		virtual void OnChildEventRButtonUp(CBkOwnerEventDlg& ownerDlg, CBkWindow& child, const CPoint& point) {}
		virtual void OnChildEventMouseHover(CBkOwnerEventDlg& ownerDlg, CBkWindow& child, const CPoint& point) {}
		virtual void OnChildEventMouseLeave(CBkOwnerEventDlg& ownerDlg, CBkWindow& child) {}
		virtual void OnChildEventMouseWheel(CBkOwnerEventDlg& ownerDlg, CBkWindow& child, int nDeta, UINT nFlags, const CPoint& point) {}
		virtual void OnChildEventDblClk(CBkOwnerEventDlg& ownerDlg, CBkWindow& child, const CPoint& point) {}
		virtual void OnChildEventMouseMove(CBkOwnerEventDlg& ownerDlg, CBkWindow& child, const CPoint& point) {}
		
	};

	CBkOwnerEventDlg() 
	{
		m_hBkWndHover = m_hBkWndPushDown = m_hBkWndRPushDown = NULL;
		m_pIChildCtrlEventListener = NULL;
	}

	void SetListener(IChildCtrlEventListener* pListener) 
	{
		m_pIChildCtrlEventListener = pListener;
	}

	virtual HBKWND BkGetHWNDFromPoint(POINT ptHitTest, BOOL bOnlyText)
	{
		if (m_rcWindow.PtInRect(ptHitTest))
		{
			return m_hBkWnd;
		}

		return NULL;
	}

	virtual LPCWSTR GetToolTipText() {
		if (m_hBkWndHover)
		{
			CBkWindow* pWndHover = BkWnds::GetWindow(m_hBkWndHover);
			if (pWndHover)
			{
				return pWndHover->GetToolTipText();
			}
		}

		return CBkWindow::GetToolTipText();
	}

	virtual void SetCursor()
	{
		if (m_hBkWndHover)
		{
			CBkWindow *pBkWndHover = BkWnds::GetWindow(m_hBkWndHover);

			if (pBkWndHover)
			{
				if (!pBkWndHover->IsDisabled(TRUE))
				{
					pBkWndHover->SetCursor();
					return;
				}
			}
		}

		__super::SetCursor();
	}

protected:
	HBKWND GetChildHWNDFromPoint(POINT ptHitTest, BOOL bOnlyText)
	{
		POSITION pos = NULL;
		HBKWND   hBkWndChild = NULL;

		pos = m_lstWndChild.GetTailPosition();
		while (pos != NULL)
		{
			CBkWindow *pBkWndChild = m_lstWndChild.GetPrev(pos);

			if (!pBkWndChild->IsVisible() || pBkWndChild->IsTransparent())
			{
				continue;
			}

			hBkWndChild = pBkWndChild->BkGetHWNDFromPoint(ptHitTest, bOnlyText);
			if (hBkWndChild)
				break;
		}

		return hBkWndChild;
	}

	void OnMouseMove(UINT nFlags, CPoint point)
	{
		if (m_hBkWndPushDown)
		{
			CBkWindow* pWndHover = BkWnds::GetWindow(m_hBkWndPushDown);
			if (pWndHover)
			{
				pWndHover->BkSendMessage(WM_MOUSEMOVE, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));
				if (m_pIChildCtrlEventListener != NULL) 
				{
					m_pIChildCtrlEventListener->OnChildEventMouseMove(*this, *pWndHover, point);
				}
				return;
			}
		}

		HBKWND hBkWndHitTest = GetChildHWNDFromPoint(point, TRUE);
		CBkWindow* pWndHover = BkWnds::GetWindow(hBkWndHitTest);

		if (pWndHover)
		{
			BOOL bDisabled = pWndHover->IsDisabled(TRUE);
			if (!bDisabled)
			{
				pWndHover->BkSendMessage(WM_MOUSEMOVE, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));
				if (m_pIChildCtrlEventListener != NULL) 
				{
					m_pIChildCtrlEventListener->OnChildEventMouseMove(*this, *pWndHover, point);
				}
			}

			if (hBkWndHitTest != m_hBkWndHover)
			{
				if (NULL != m_hBkWndHover)
				{
					CBkWindow* pWndHoverOld = BkWnds::GetWindow(m_hBkWndHover);

					if (pWndHoverOld && !pWndHoverOld->IsDisabled(TRUE))
					{
						_ModifyWndState(pWndHoverOld, 0, BkWndState_Hover);
						pWndHoverOld->BkSendMessage(WM_MOUSELEAVE);
						// notify mouse leave
						if (m_pIChildCtrlEventListener != NULL) 
						{
							m_pIChildCtrlEventListener->OnChildEventMouseLeave(*this, *pWndHoverOld);
						}
					}
				}

				m_hBkWndHover = hBkWndHitTest;

				if (!bDisabled) 
				{
					_ModifyWndState(pWndHover, BkWndState_Hover, 0);
					::SendMessage(m_hWndContainer, WM_BK_UPDATE_TIP_TEXT, (WPARAM)(pWndHover->GetToolTipText()), NULL);
					// notify hover
					if (m_pIChildCtrlEventListener != NULL) 
					{
						m_pIChildCtrlEventListener->OnChildEventMouseHover(*this, *pWndHover, point);
					}
				}
			}
		}
		else if (hBkWndHitTest != m_hBkWndHover)
		{
			if (NULL != m_hBkWndHover)
			{
				CBkWindow* pWndHoverOld = BkWnds::GetWindow(m_hBkWndHover);

				if (pWndHoverOld && !pWndHoverOld->IsDisabled(TRUE))
				{
					_ModifyWndState(pWndHoverOld, 0, BkWndState_Hover);
					pWndHoverOld->BkSendMessage(WM_MOUSELEAVE);
					// notify mouseleave
					if (m_pIChildCtrlEventListener != NULL) 
					{
						m_pIChildCtrlEventListener->OnChildEventMouseLeave(*this, *pWndHoverOld);
					}
				}
			}

			m_hBkWndHover = NULL;
		}
	}

	void OnLButtonDown(UINT nFlags, CPoint point)
	{
		HBKWND hBkWndHitTest = GetChildHWNDFromPoint(point, TRUE);
		CBkWindow* pWndPushDown = BkWnds::GetWindow(hBkWndHitTest);

		if (pWndPushDown)
		{
			if (pWndPushDown->IsDisabled(TRUE))
				return;

			m_hBkWndPushDown = hBkWndHitTest;
			_ModifyWndState(pWndPushDown, BkWndState_PushDown, 0);
			pWndPushDown->BkSendMessage(WM_LBUTTONDOWN, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));
			// notiy WM_LBUTTONDOWN
			if (m_pIChildCtrlEventListener != NULL) 
			{
				m_pIChildCtrlEventListener->OnChildEventLButtonDown(*this, *pWndPushDown, point);
			}
		}
	}

	void OnLButtonUp(UINT nFlags, CPoint point)
	{
		UINT uCmdID;
		CBkWindow* pWndClick = NULL;
		BOOL bNotifyClick = FALSE;
		BOOL bNotifyMouseLeave = FALSE;

		HBKWND hBkWndHitTest = GetChildHWNDFromPoint(point, TRUE);
		HBKWND hBkWndPushDown = m_hBkWndPushDown;

		if (m_hBkWndPushDown)
		{
			pWndClick = BkWnds::GetWindow(m_hBkWndPushDown);

			if (pWndClick)
			{
				uCmdID = pWndClick->GetCmdID();

				_ModifyWndState(pWndClick, 0, BkWndState_PushDown);
				pWndClick->BkSendMessage(WM_LBUTTONUP, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));

				if (m_hBkWndPushDown == hBkWndHitTest)
				{		
					bNotifyClick = TRUE;
				}
				else
				{
					_ModifyWndState(pWndClick, 0, BkWndState_Hover);
					pWndClick->BkSendMessage(WM_MOUSELEAVE, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));
					bNotifyMouseLeave = TRUE;
				}
			}

			m_hBkWndPushDown = NULL;
		}

		if (bNotifyClick)
		{
			if (m_pIChildCtrlEventListener != NULL) 
			{
				m_pIChildCtrlEventListener->OnChildEventLButtonUp(*this, *pWndClick, point);
			}
		}
		else if (bNotifyMouseLeave)
		{
			if(hBkWndPushDown == m_hBkWndHover)
				m_hBkWndHover = NULL;
			// pWndClick notify mouse leave
			if (m_pIChildCtrlEventListener != NULL) 
			{
				m_pIChildCtrlEventListener->OnChildEventMouseLeave(*this, *pWndClick);
			}
		}
	}

	void OnMouseLeave()
	{
		CBkWindow* pWnd = NULL;
		if (m_hBkWndHover)
		{
			CBkWindow* pWndHover = BkWnds::GetWindow(m_hBkWndHover);
			if (pWndHover)
			{
				_ModifyWndState(pWndHover, 0, BkWndState_Hover);
				pWndHover->BkSendMessage(WM_MOUSELEAVE);
			}

			m_hBkWndHover = NULL;
			pWnd = pWndHover;
		}

		if (m_hBkWndPushDown)
		{
			CBkWindow* pWndPushdown = BkWnds::GetWindow(m_hBkWndPushDown);
			if (pWndPushdown)
				_ModifyWndState(pWndPushdown, 0, BkWndState_PushDown);

			m_hBkWndPushDown = NULL;
			pWnd = pWndPushdown;
		}

		m_hBkWndRPushDown = NULL;
		if (pWnd)
		{
			if (m_pIChildCtrlEventListener != NULL) 
			{
				m_pIChildCtrlEventListener->OnChildEventMouseLeave(*this, *pWnd);
			}
		}
	}

	void OnLButtonDblClk(UINT nFlags, CPoint point)
	{
		CBkWindow* pWndPushDown = NULL;
		HBKWND hBkWndHitTest = GetChildHWNDFromPoint(point, TRUE);

		pWndPushDown = (hBkWndHitTest ? BkWnds::GetWindow(hBkWndHitTest) : NULL);
		if (pWndPushDown)
		{
			pWndPushDown->BkSendMessage(WM_LBUTTONDBLCLK, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));
			//pWndPushDown nitify MLBUTTON_DBCLICKOUSELEAVE
			if (m_pIChildCtrlEventListener != NULL) 
			{
				m_pIChildCtrlEventListener->OnChildEventDblClk(*this, *pWndPushDown, point);
			}
		}		
	}

	BOOL OnMouseWhell( UINT nFlags, short zDelta, CPoint point )
	{
		BOOL bSuccess = FALSE;
		CBkWindow* pWnd = BkWnds::GetWindow(m_hBkWndHover);
		if (pWnd)
		{
			bSuccess = pWnd->BkSendMessage(WM_MOUSEWHEEL, MAKEWPARAM(nFlags, zDelta), MAKELPARAM(point.x, point.y));
			if (m_pIChildCtrlEventListener != NULL) 
			{
				m_pIChildCtrlEventListener->OnChildEventMouseWheel(*this, *pWnd, zDelta, nFlags, point);
			}
		}

		return bSuccess;
	}

	void OnRButtonDown(UINT nFlags, CPoint point)
	{
		HBKWND hBkWndHitTest = GetChildHWNDFromPoint(point, TRUE);

		CBkWindow* pWndPushDown = BkWnds::GetWindow(hBkWndHitTest);
		if (pWndPushDown)
		{
			m_hBkWndRPushDown = hBkWndHitTest;

			pWndPushDown->BkSendMessage(WM_RBUTTONDOWN, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));
			// pWndPushDown RBUTTONDOWN
			if (m_pIChildCtrlEventListener != NULL) 
			{
				m_pIChildCtrlEventListener->OnChildEventRButtonDown(*this, *pWndPushDown, point);
			}
		}
	}

	void OnRButtonUp(UINT nFlags, CPoint point)
	{
		HBKWND hBkWndHitTest = GetChildHWNDFromPoint(point, TRUE);

		if (m_hBkWndRPushDown == hBkWndHitTest)
		{
			CBkWindow* pWndPushDown = BkWnds::GetWindow(hBkWndHitTest);
			if (pWndPushDown)
			{
				pWndPushDown->BkSendMessage(WM_RBUTTONUP, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));
				// pWndPushDown RBUTTONUP
				if (m_pIChildCtrlEventListener != NULL) 
				{
					m_pIChildCtrlEventListener->OnChildEventRButtonUp(*this, *pWndPushDown, point);
				}
			}
		}

		m_hBkWndRPushDown = NULL;
	}

	BOOL OnSetCursor(CWindow /*wnd*/, UINT nHitTest, UINT message)
	{
		SetCursor();

		return TRUE;
	}

protected:

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
	BKWIN_BEGIN_MSG_MAP()
		MSG_WM_MOUSEWHEEL(OnMouseWhell)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_RBUTTONDOWN(OnRButtonDown)
		MSG_WM_RBUTTONUP(OnRButtonUp)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
		MSG_WM_SETCURSOR(OnSetCursor)
	BKWIN_END_MSG_MAP()

protected:
	HBKWND			  m_hBkWndHover;
	HBKWND			  m_hBkWndPushDown;
	HBKWND			  m_hBkWndRPushDown;
	IChildCtrlEventListener* m_pIChildCtrlEventListener;
};

#endif