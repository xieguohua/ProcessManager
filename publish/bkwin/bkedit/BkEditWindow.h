//********************************************************************
//author:  hexujun@cmcm.com
//create time: 2020/6/16?21:21
//description: √Ë ˆ
//*********************************************************************

#ifndef _H_BKEDITWINDOW_H_
#define _H_BKEDITWINDOW_H_

#include "..\bkwnd.h"
#include "BkTextServices.h"
#include "IBkTimer.h"
#include "..\bkbitmap.h"

class CBkEditWindow 
	: public CBkWindow
	, public IBkRichTextWindow
	, public BkTimer::IBkTimerNotify
{
public:

	BKOBJ_DECLARE_CLASS_NAME(CBkEditWindow, "edit")

public:

	CBkEditWindow();
	~CBkEditWindow();
	
	virtual BOOL Load(TiXmlElement* pTiXmlElem);

	void SetIBkWndTimer(BkTimer::IBkWndTimer* pTimer);
	CString GetEditText();
	void SetEditText(LPCWSTR lpszText);

protected:
	virtual HWND GetHWND();
	virtual CRect GetViewInset();
	virtual CRect GetWindowRect();
	virtual void Invalidate();
protected:
	virtual void OnBkTimer(UINT uItmerID);


protected:
	virtual HRESULT SetAttribute(CStringA strAttribName,   CStringA strValue, BOOL  bLoading);      

private:
	void UpdateTextServiceFont();
	void UpdateTextServiceTextColor();

protected:
	BKWIN_BEGIN_MSG_MAP(CBkEditWindow)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_PAINT(OnPaint)
		MESSAGE_HANDLER_EX(WM_CHAR, OnMsgDispathFocusWnd)
		MESSAGE_HANDLER_EX(WM_KEYDOWN, OnMsgDispathFocusWnd)
		MESSAGE_HANDLER_EX(WM_KEYUP, OnMsgDispathFocusWnd)
		MESSAGE_HANDLER_EX(WM_LBUTTONDBLCLK, OnMsgDispathFocusWnd)
		MESSAGE_HANDLER_EX(WM_RBUTTONUP, OnMsgDispathFocusWnd)
		MESSAGE_HANDLER_EX(WM_RBUTTONDOWN, OnMsgDispathFocusWnd)
		MESSAGE_HANDLER_EX(WM_RBUTTONDBLCLK, OnMsgDispathFocusWnd)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_SETFOCUS(OnSetFocus)
		MSG_WM_KILLFOCUS(OnKillFocus)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
		MSG_WM_WINDOWPOSCHANGED(OnWindowPosChanged)
	BKWIN_END_MSG_MAP()

protected:
	void OnDestroy();
	void OnPaint(CDCHandle dc);
	LRESULT OnMsgDispathFocusWnd(UINT uMsg, WPARAM wParam, LPARAM lParam);
	 void OnLButtonDown(UINT nFlags, CPoint point);
	 void OnLButtonUp(UINT nFlags, CPoint point);
	 void OnSetFocus(CWindow wndOld);
	 void OnKillFocus(CWindow wndFocus);
	 void OnMouseMove(UINT nFlags, CPoint point);
	 void OnMouseLeave();
	 void OnWindowPosChanged(LPWINDOWPOS lpWndPos);

protected:

	bool PrepareBmp();
	bool ValidateTextService() const;
	void TxSendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	CPoint ConvertToLocalEditPos(const CPoint& pt);

private:
	CBkTextServiceHost* m_pTextServiceHost;
	CString m_strEditText;
	CBkBitmap m_bkbmp;
};

#endif