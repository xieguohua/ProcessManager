#pragma once

#include "bkres/bkres.h"
#include "bkwin/CBkDialogViewImplEx.h"
#include "bkwin/bkshadowdrawable.h"
#include "bkwin/CBkDialogMenu.h"
#include "KProcListWnd.h"
#include "UsageDetector.h"

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
        IDC_BTN_SIZE = 1001,
        IDC_BTN_MIN = 1002,
        IDC_BTN_SETTING = 1003,
        IDC_BTN_KILL_PROCESS = 1004,
        
        IDC_TEXT_PROCESS_COUNT = 2000,
        IDC_TEXT_CPU_USAGE = 2001,
        IDC_TEXT_MEMORY_USAGE = 2002,

        TIMER_UPDATE_USAGE = 1000,

		IDC_PROC_LIST_WND = 3000,
    };

protected:
    BK_NOTIFY_MAP(IDC_RICHVIEW_WIN_EX)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_CLOSE, OnBtnClose)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_SIZE, OnBtnSize)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_MIN, OnBtnMin)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_SETTING, OnBtnSetting)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_KILL_PROCESS, OnBtnKillProcess)
    BK_NOTIFY_MAP_END()

    BEGIN_MSG_MAP_EX(CBkDialogViewImplEx<KMainDlg>)
        MSG_BK_NOTIFY(IDC_RICHVIEW_WIN_EX)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_SYSCOMMAND(OnSysCommand)
        MSG_WM_TIMER(OnTimer)
		CHAIN_MSG_MAP(CBkDialogViewImplEx<KMainDlg>)
        REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()

    BOOL OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/);
    void OnBtnClose();
    void OnBtnSize();
    void OnBtnMin();
    void OnBtnSetting();
    void OnBtnKillProcess();
    void OnSysCommand(UINT nID, CPoint point);
    void OnTimer(UINT_PTR uTimerId);

private:
    void UpdateSetting();
    void UpdateUsage();

private:
    UsageDetector m_usageDetector;
	KProcListWnd m_wndProcList;
};