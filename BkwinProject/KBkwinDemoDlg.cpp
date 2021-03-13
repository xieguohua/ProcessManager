#include "stdafx.h"
#include "KBkwinDemoDlg.h"
#include "DemoListWndDlg.h"
#include "DemoCmnCtrlSetDlg.h"

KBkwinDemoDlg::KBkwinDemoDlg()
    : CBkDialogViewImplEx<KBkwinDemoDlg>(IDR_MAIN)
{
	BkWin::WndShadowHelper<KBkwinDemoDlg>::SetShadowData(12, IDP_SHADOW);
	m_pMenu = NULL;
}

KBkwinDemoDlg::~KBkwinDemoDlg()
{
	if(m_pMenu) 
	{
		if (m_pMenu->IsWindow()) {
			m_pMenu->DestroyWindow();
		}
		delete m_pMenu;
		m_pMenu = NULL;
	}
}

BOOL KBkwinDemoDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{
    return TRUE;
}

void KBkwinDemoDlg::OnBtnClose()
{
    EndDialog(IDCLOSE);
}

void KBkwinDemoDlg::OnSysCommand(UINT nID, CPoint point)
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

void KBkwinDemoDlg::OnBtnDemoListWnd()
{
	DemoListWndDlg dlg;
	dlg.DoModal(m_hWnd);
}

void KBkwinDemoDlg::OnBtnDemoMenu()
{
	if (m_pMenu == NULL) {
		m_pMenu = new CBkDialogMenu();
		m_pMenu->SetCmdListener(this);
		m_pMenu->CreateMenu(m_hWnd, IDR_IMAGE_CONTENT_MENULAYOUT);
	}

	CPoint pt;
	::GetCursorPos(&pt);
	m_pMenu->PopUp(pt);
}

void KBkwinDemoDlg::OnBtnCtrlSet()
{
	DemoCmnCtrlSetDlg dlg;
	dlg.DoModal(m_hWnd);
}

void KBkwinDemoDlg::OnBkMenuCmd(CBkDialogMenu* pDialogMenu, LPCWSTR lpszMenuName)
{	
	CString strCmd;

	strCmd.Format(L"OnBkMenuCmd name=%s", lpszMenuName);
	MessageBox(strCmd);
}
