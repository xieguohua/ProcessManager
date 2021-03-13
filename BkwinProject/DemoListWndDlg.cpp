#include "stdafx.h"
#include "DemoListWndDlg.h"

DemoListWndDlg::DemoListWndDlg()
    : CBkDialogViewImplEx<DemoListWndDlg>(IDR_DLG_DEMO_LISTWND)
{
	BkWin::WndShadowHelper<DemoListWndDlg>::SetShadowData(12, IDP_SHADOW);
}

BOOL DemoListWndDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{
    return TRUE;
}

void DemoListWndDlg::OnBtnClose()
{
    EndDialog(IDCLOSE);
}

void DemoListWndDlg::OnSysCommand(UINT nID, CPoint point)
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

void DemoListWndDlg::OnListItemLBtnUp(int nListItem)
{
	BkSharePtr<CString> strMsg = BkSharePtr<CString>::createObject();

	strMsg.DataRef().Format(L"DemoListWndDlg::OnListItemLBtnUp:%d", nListItem);
	PostRunnable(BkWin::FuncFactory::create_class_func(this, &DemoListWndDlg::ShowDoModalMsgBox, strMsg));
}

void DemoListWndDlg::ShowDoModalMsgBox(BkSharePtr<CString> msg)
{
	MessageBox(msg.DataRef());
}

void DemoListWndDlg::OnListItemChildLBtnUp(int nListItem, int nChildCtrlId)
{
	BkSharePtr<CString> strMsg = BkSharePtr<CString>::createObject();
	
	strMsg.DataRef().Format(L"DemoListWndDlg::OnListItemChildLBtnUp:%d %d", nListItem, nChildCtrlId);
	PostRunnable(BkWin::FuncFactory::create_class_func(this, &DemoListWndDlg::ShowDoModalMsgBox, strMsg));
}


