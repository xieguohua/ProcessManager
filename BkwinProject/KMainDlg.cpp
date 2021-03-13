#include "stdafx.h"
#include "KMainDlg.h"
#include "DemoListWndDlg.h"
#include "DemoCmnCtrlSetDlg.h"
#include "KDlgSetting.h"

KMainDlg::KMainDlg()
    : CBkDialogViewImplEx<KMainDlg>(IDR_MAIN)
{
	BkWin::WndShadowHelper<KMainDlg>::SetShadowData(12, IDP_SHADOW);
}

KMainDlg::~KMainDlg()
{
}

BOOL KMainDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{
    return TRUE;
}

void KMainDlg::OnBtnClose()
{
    EndDialog(IDCLOSE);
}

void KMainDlg::OnSysCommand(UINT nID, CPoint point)
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

void KMainDlg::OnBtnSetting()
{
    KDlgSetting dlg;
    dlg.DoModal();
}
