#include "stdafx.h"
#include "KDlgSetting.h"
#include "DemoListWndDlg.h"
#include "DemoCmnCtrlSetDlg.h"

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