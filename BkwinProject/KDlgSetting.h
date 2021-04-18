#pragma once

#include "bkres/bkres.h"
#include "bkwin/CBkDialogViewImplEx.h"
#include "bkwin/bkshadowdrawable.h"
#include "bkwin/CBkDialogMenu.h"
#include "SettingDefine.h"

class KDlgSetting
    : public CBkDialogViewImplEx<KDlgSetting>
	, public BkWin::WndShadowHelper<KDlgSetting>
{
public:
    KDlgSetting();
	~KDlgSetting();
    static std::vector<ENUM_SETTING> GetSettingOption();

    enum
    {
        IDC_BTN_CLOSE = 1000,
        IDC_BTN_RESTORE = 1001,
        IDC_BTN_CONFIRM = 1002,
        IDC_BTN_CANCEL = 1003,

        IDC_CHECK_START = 2000,
        IDC_CHECK_PARENT_ID,
        IDC_CHECK_SESSION_ID,
        IDC_CHECK_CPU_USAGE,
        IDC_CHECK_MEMORY_USAGE,
        IDC_CHECK_HANDLE_COUND,
        IDC_CHECK_CREATE_TIME,
        IDC_CHECK_PROCESS_PATH,
        IDC_CHECK_USER_COUNT,
        IDC_CHECK_GDI_COUNT,
        //IDC_CHECK_THREAD_COUNT,
        IDC_CHECK_END,
    };

protected:
    BK_NOTIFY_MAP(IDC_RICHVIEW_WIN_EX)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_CLOSE, OnBtnClose)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_RESTORE, OnBtnRestore)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_CONFIRM, OnBtnConfirm)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_CANCEL, OnBtnCancel)
    BK_NOTIFY_MAP_END()

    BEGIN_MSG_MAP_EX(CBkDialogViewImplEx<KDlgSetting>)
        MSG_BK_NOTIFY(IDC_RICHVIEW_WIN_EX)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_SYSCOMMAND(OnSysCommand)
		CHAIN_MSG_MAP(CBkDialogViewImplEx<KDlgSetting>)
        REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()

    BOOL OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/);
    void OnSysCommand(UINT nID, CPoint point);
    void OnBtnClose();
    void OnBtnRestore();
    void OnBtnConfirm();
    void OnBtnCancel();

private:
    void InitOption();
    void SetDefaultOption();
    void UpdateOption(const std::vector<ENUM_SETTING>& vecChecked);
    ENUM_SETTING TransformSettingOption(int nCheckViewId);
    int TransformCheckViewId(ENUM_SETTING settingOption);
    void GetCheckedOption(std::vector<ENUM_SETTING>& vecOption);
    void RecordSettingOption();
    static BOOL GetSettingOptionByReg(std::vector<ENUM_SETTING>& vecOption);

private:
    std::vector<ENUM_SETTING> m_vecCurSetting;
};