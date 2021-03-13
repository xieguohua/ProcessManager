/*************************************************
//  Copyright (C), 2017-2027, CS&S. Co., Ltd.
//  File name:       DemoCmnCtrlSetDlg.h.h  
//  Version:          1.0     
//  Date:              2021/01/10
//    <author>      hexujun
//  mail:           504309952@qq.com 
*************************************************/
#ifndef __RB_UI_DemoCmnCtrlSetDlg_h_
#define __RB_UI_DemoCmnCtrlSetDlg_h_

#include "bkres/bkres.h"
#include "bkwin/CBkDialogViewImplEx.h"
#include "bkwin/bkshadowdrawable.h"
#include "bkwin/BkDialogViewExtend.h"

class DemoCmnCtrlSetDlg
	: public BkWin::BkDialogViewExtend<DemoCmnCtrlSetDlg>
	, public BkWin::WndShadowHelper<DemoCmnCtrlSetDlg>
{
public:
	DemoCmnCtrlSetDlg();

	enum
	{
		IDC_BTN_CLOSE = 1000,
		IDC_LISTWND = 2000
	};

protected:
	BK_NOTIFY_MAP(IDC_RICHVIEW_WIN_EX)
		BK_NOTIFY_ID_COMMAND(IDC_BTN_CLOSE, OnBtnClose)
	BK_NOTIFY_MAP_END()

	BEGIN_MSG_MAP_EX(DemoCmnCtrlSetDlg)
		MSG_BK_NOTIFY(IDC_RICHVIEW_WIN_EX)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_SYSCOMMAND(OnSysCommand)
		CHAIN_MSG_MAP(BkWin::BkDialogViewExtend<DemoCmnCtrlSetDlg>)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/);
	void OnBtnClose();
	void OnSysCommand(UINT nID, CPoint point);

};

#endif //__RB_UI_DemoCmnCtrlSetDlg_h_
