#pragma once

#include "bkres/bkres.h"
#include "bkwin/CBkDialogViewImplEx.h"
#include "bkwin/bkshadowdrawable.h"
#include "bkwin/CBkDialogMenu.h"

class KMainDlg
    : public CBkDialogViewImplEx<KMainDlg>
	, public BkWin::WndShadowHelper<KMainDlg>
{
public:
    KMainDlg();
	~KMainDlg();

    enum
    {
        IDC_BTN_CLOSE = 1000,
        IDC_BTN_SETTING = 1001,

		IDC_BTN_CTRL_SET = 2000,

		IDC_BTN_DEMO_LISTWND = 2001,
		IDC_BTN_DEMO_MENU = 2002,
    };

protected:
    BK_NOTIFY_MAP(IDC_RICHVIEW_WIN_EX)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_CLOSE, OnBtnClose)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_SETTING, OnBtnSetting)
    BK_NOTIFY_MAP_END()

    BEGIN_MSG_MAP_EX(CBkDialogViewImplEx<KMainDlg>)
        MSG_BK_NOTIFY(IDC_RICHVIEW_WIN_EX)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_SYSCOMMAND(OnSysCommand)
		CHAIN_MSG_MAP(CBkDialogViewImplEx<KMainDlg>)
        REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()

    BOOL OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/);
    void OnBtnClose();
    void OnBtnSetting();
    void OnSysCommand(UINT nID, CPoint point);
};