#include "stdafx.h"
#include "KMainDlg.h"
#include "DemoListWndDlg.h"
#include "DemoCmnCtrlSetDlg.h"
#include "KDlgSetting.h"

namespace
{
    const int UPDATE_USAGE_INTERVAL = 1000;
    const LPCTSTR TEXT_PROCESS_COUNT = L"正在运行的进程：%d个";
    const LPCTSTR TEXT_CPU_USAGE = L"CPU占用：%d%%";
    const LPCTSTR TEXT_MEMORY_USAGE = L"内存占用：%d%%";
}

KMainDlg::KMainDlg()
    : CBkDialogViewImplEx<KMainDlg>(IDR_MAIN)
{
	BkWin::WndShadowHelper<KMainDlg>::SetShadowData(12, IDP_SHADOW);
    m_usageDetector.Initialize();
}

KMainDlg::~KMainDlg()
{
    m_usageDetector.Uninitialize();
}

BOOL KMainDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{
    SetTimer(TIMER_UPDATE_USAGE, UPDATE_USAGE_INTERVAL);
	m_wndProcList.CreateEx(m_hWnd, IDC_PROC_LIST_WND);
    return TRUE;
}

void KMainDlg::OnBtnClose()
{
    EndDialog(IDCLOSE);
}

void KMainDlg::OnBtnSize()
{
    if (IsZoomed())
    {
        ShowWindow(SW_RESTORE);
        SetItemAttribute(IDC_BTN_SIZE, "skin", "maxbtn");
    }
    else
    {
        ShowWindow(SW_SHOWMAXIMIZED);
        SetItemAttribute(IDC_BTN_SIZE, "skin", "restorebtn");
    }
}

void KMainDlg::OnBtnMin()
{
    ShowWindow(SW_SHOWMINIMIZED);
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
    UINT_PTR nRet = dlg.DoModal();
    if (nRet == IDOK)
    {
        UpdateSetting();
    }
}

void KMainDlg::OnBtnKillProcess()
{

}

void KMainDlg::OnTimer(UINT_PTR uTimerId)
{
    if (TIMER_UPDATE_USAGE == uTimerId)
    {
        UpdateUsage();
    }
}

void KMainDlg::UpdateSetting()
{
    m_wndProcList.UpdateSetting();
}

void KMainDlg::UpdateUsage()
{
    CString strProcess;
    CString strCpuUsage;
    CString strMemoryUsage;
    strProcess.Format(TEXT_PROCESS_COUNT, DataCenter::GetInstance()->GetProcCount());
    strCpuUsage.Format(TEXT_CPU_USAGE, m_usageDetector.GetCpuUsage());
    strMemoryUsage.Format(TEXT_MEMORY_USAGE, m_usageDetector.GetMemoryUsage());
    SetItemText(IDC_TEXT_PROCESS_COUNT, strProcess);
    SetItemText(IDC_TEXT_CPU_USAGE, strCpuUsage);
    SetItemText(IDC_TEXT_MEMORY_USAGE, strMemoryUsage);
}
