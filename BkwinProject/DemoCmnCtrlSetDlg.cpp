#include "stdafx.h"
#include "DemoCmnCtrlSetDlg.h"

DemoCmnCtrlSetDlg::DemoCmnCtrlSetDlg()
    : BkWin::BkDialogViewExtend<DemoCmnCtrlSetDlg>(IDR_DLG_CTRL_SET)
{
	BkWin::WndShadowHelper<DemoCmnCtrlSetDlg>::SetShadowData(12, IDP_SHADOW);
}

BOOL DemoCmnCtrlSetDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{
    return TRUE;
}

void DemoCmnCtrlSetDlg::OnBtnClose()
{
    EndDialog(IDCLOSE);
}

void DemoCmnCtrlSetDlg::OnSysCommand(UINT nID, CPoint point)
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



