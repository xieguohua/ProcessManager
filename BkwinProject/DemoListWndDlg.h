/*************************************************
//  Copyright (C), 2017-2027, CS&S. Co., Ltd.
//  File name:       DemoListWndDlg.h  
//  Version:          1.0     
//  Date:              2021/01/10
//    <author>      hexujun
//  mail:           504309952@qq.com 
*************************************************/
#ifndef __RB_UI_DemoListWndDlg_h_
#define __RB_UI_DemoListWndDlg_h_

#include "bkres/bkres.h"
#include "bkwin/CBkDialogViewImplEx.h"
#include "bkwin/bkshadowdrawable.h"
#include "bkwin/BkWndMsgFunction.h"

class DemoListWndDlg
	: public CBkDialogViewImplEx<DemoListWndDlg>
	, public BkWin::WndShadowHelper<DemoListWndDlg>
	, public BkWin::CBkWndMsgFunction<DemoListWndDlg>
{
public:
	DemoListWndDlg();

	enum
	{
		IDC_BTN_CLOSE = 1000,
		IDC_LISTWND = 2000
	};

protected:
	BK_NOTIFY_MAP(IDC_RICHVIEW_WIN_EX)
		BK_NOTIFY_ID_COMMAND(IDC_BTN_CLOSE, OnBtnClose)
		BK_LISTWND_NOTIFY_BEGIN(IDC_LISTWND)
			BK_LISTWND_LISTITEM_LBUTTONUP(OnListItemLBtnUp)
			BK_LISTWND_LISTITEM_CHILD_LBUTTONUP(OnListItemChildLBtnUp)
		BK_LISTWND_NOTIFY_END()
	BK_NOTIFY_MAP_END()

	BEGIN_MSG_MAP_EX(DemoListWndDlg)
		MSG_BK_NOTIFY(IDC_RICHVIEW_WIN_EX)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_SYSCOMMAND(OnSysCommand)
		CHAIN_MSG_MAP(BkWin::CBkWndMsgFunction<DemoListWndDlg>)
		CHAIN_MSG_MAP(CBkDialogViewImplEx<DemoListWndDlg>)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/);
	void OnBtnClose();
	void OnSysCommand(UINT nID, CPoint point);

	void OnListItemLBtnUp(int nListItem);
	void OnListItemChildLBtnUp(int nListItem, int nChildCtrlId);

protected:
	void ShowDoModalMsgBox(BkSharePtr<CString> msg);
};

#endif //__RB_UI_DemoListWndDlg_h_
