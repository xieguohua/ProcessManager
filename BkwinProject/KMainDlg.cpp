#include "stdafx.h"
#include "KMainDlg.h"
#include "DemoListWndDlg.h"
#include "DemoCmnCtrlSetDlg.h"
#include "KDlgSetting.h"

KMainDlg::KMainDlg()
    : CBkDialogViewImplEx<KMainDlg>(IDR_MAIN)
{
	BkWin::WndShadowHelper<KMainDlg>::SetShadowData(12, IDP_SHADOW);
	m_pMenu = NULL;
}

KMainDlg::~KMainDlg()
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

void KMainDlg::OnBtnDemoListWnd()
{
	DemoListWndDlg dlg;
	dlg.DoModal(m_hWnd);
}

void KMainDlg::OnBtnDemoMenu()
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

void KMainDlg::OnBtnCtrlSet()
{
	DemoCmnCtrlSetDlg dlg;
	dlg.DoModal(m_hWnd);
}

void KMainDlg::OnBkMenuCmd(CBkDialogMenu* pDialogMenu, LPCWSTR lpszMenuName)
{	
	CString strCmd;

	strCmd.Format(L"OnBkMenuCmd name=%s", lpszMenuName);
	MessageBox(strCmd);
}

void KMainDlg::OnBtnSetting()
{
    KDlgSetting dlg;
    dlg.DoModal();
}
