#include "stdafx.h"
#include "KDlgSetting.h"
#include "framework/KRegister2.h"
#include "KUtils.h"

namespace
{
    const HKEY SETTING_REG_ROOT_KEY = HKEY_CURRENT_USER;
    const LPCTSTR SETTING_REG_PATH = L"software\\processmanager";
    const LPCTSTR SETTING_REG_KEY = L"setting_option";
    const ENUM_SETTING DEFAULT_CHECKED_LIST[] = {
        SETTING_CPU_USAGE,
        SETTING_MEMORY_USAGE,
        SETTING_PROCESS_PATH,
    };
}

KDlgSetting::KDlgSetting()
    : CBkDialogViewImplEx<KDlgSetting>(IDR_DLG_SETTING)
{
	BkWin::WndShadowHelper<KDlgSetting>::SetShadowData(12, IDP_SHADOW);
}

KDlgSetting::~KDlgSetting()
{
}

BOOL KDlgSetting::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{
    InitOption();
    return TRUE;
}

void KDlgSetting::OnBtnClose()
{
    EndDialog(IDCLOSE);
}

void KDlgSetting::OnSysCommand(UINT nID, CPoint point)
{
    if (nID == SC_CLOSE)
    {
        if( ::IsWindowEnabled(m_hWnd))
        {
            OnBtnClose();
        }
    }
    else
    {
        SetMsgHandled(FALSE);
    }
}

void KDlgSetting::OnBtnRestore()
{
    SetDefaultOption();
}

void KDlgSetting::OnBtnConfirm()
{
    RecordSettingOption();
    EndDialog(IDOK);
}

void KDlgSetting::OnBtnCancel()
{
    EndDialog(IDCANCEL);
}

void KDlgSetting::InitOption()
{
    std::vector<ENUM_SETTING> vecOption = GetSettingOption();
    UpdateOption(vecOption);
}

void KDlgSetting::SetDefaultOption()
{
    std::vector<ENUM_SETTING> vecCheckedOption;
    for (int nIndex = 0; nIndex < _countof(DEFAULT_CHECKED_LIST); ++nIndex)
    {
        vecCheckedOption.push_back(DEFAULT_CHECKED_LIST[nIndex]);
    }

    UpdateOption(vecCheckedOption);
}

void KDlgSetting::UpdateOption(const std::vector<ENUM_SETTING>& vecChecked)
{
    for (int nViewId = IDC_CHECK_START; nViewId < IDC_CHECK_END; ++nViewId)
    {
        SetItemCheck(nViewId, FALSE);
    }

    for (int nIndex = 0; nIndex < vecChecked.size(); ++nIndex)
    {
        ENUM_SETTING checkedOption = vecChecked[nIndex];
        int nViewId = TransformCheckViewId(checkedOption);
        SetItemCheck(nViewId, TRUE);
    }
}

ENUM_SETTING KDlgSetting::TransformSettingOption(int nCheckViewId)
{
    return static_cast<ENUM_SETTING>(nCheckViewId - IDC_CHECK_START);
}

int KDlgSetting::TransformCheckViewId(ENUM_SETTING settingOption)
{
    return settingOption + IDC_CHECK_START;
}

BOOL KDlgSetting::GetSettingOptionByReg(std::vector<ENUM_SETTING>& vecOption)
{
    KRegister2 reg;
    if (!reg.Open(HKEY_CURRENT_USER, SETTING_REG_PATH))
    {
        return FALSE;
    }

    CString strSettingOption;
    if (!reg.Read(SETTING_REG_KEY, strSettingOption))
    {
        return FALSE;
    }

    std::vector<CString> vecStrOption;
    KUtils::SplitString(strSettingOption, L"|", vecStrOption);
    for (int nIndex = 0; nIndex < vecStrOption.size(); ++nIndex)
    {
        int settingOption = _wtoi(vecStrOption[nIndex]);
        vecOption.push_back(static_cast<ENUM_SETTING>(settingOption));
    }
    return TRUE;
}

void KDlgSetting::GetCheckedOption(std::vector<ENUM_SETTING>& vecOption)
{
    for (int nViewId = IDC_CHECK_START; nViewId < IDC_CHECK_END; ++nViewId)
    {
        BOOL bChecked = GetItemCheck(nViewId);
        if (bChecked)
        {
            ENUM_SETTING settingOption = TransformSettingOption(nViewId);
            vecOption.push_back(settingOption);
        }
    }
}

void KDlgSetting::RecordSettingOption()
{
    KRegister2 reg;
    if (!reg.Open(HKEY_CURRENT_USER, SETTING_REG_PATH, FALSE))
    {
        return;
    }

    CString strSettingOption;
    std::vector<ENUM_SETTING> vecOption;
    GetCheckedOption(vecOption);
    int nSize = vecOption.size();
    for (int nIndex = 0; nIndex < nSize; ++nIndex)
    {
        CString strTemp;
        int nOption = static_cast<int>(vecOption[nIndex]);
        strTemp.Format(L"%d", nOption);
        if (nIndex != nSize - 1)
        {
            strTemp.Append(L"|");
        }

        strSettingOption.Append(strTemp);
    }

    reg.Write(SETTING_REG_KEY, strSettingOption);
}

std::vector<ENUM_SETTING> KDlgSetting::GetSettingOption()
{
    std::vector<ENUM_SETTING> vecOption;
    BOOL bRet = GetSettingOptionByReg(vecOption);
    if (!bRet)
    {
        for (int nIndex = 0; nIndex < _countof(DEFAULT_CHECKED_LIST); ++nIndex)
        {
            vecOption.push_back(DEFAULT_CHECKED_LIST[nIndex]);
        }
    }
    return vecOption;
}
