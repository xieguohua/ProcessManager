#include "StdAfx.h"
#include "KProcListWnd.h"
#include "ProcInfoMgr/ProcInfoMgr.h"
#include "bkwin/BkLinearLayout.h"
#include <shellapi.h>
#include "framework/KDubaPath.h"
#include <objbase.h>
#include <shlobj.h>

namespace
{
    const int IDM_KILL_PROCESS = 100;
    const int IDM_FILE_LOCATION = 101;
    const int IDM_FILE_ATTRIBUTE = 102;
    const int IDM_EXPORT_INFO = 103;
}

KProcListWnd::KProcListWnd(void)
		: CBkDialogViewImplEx<KProcListWnd>(IDR_PROC_LIST_WND)
		, m_pListWnd(NULL)
{
}

KProcListWnd::~KProcListWnd(void)
{
}

BOOL KProcListWnd::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_pListWnd = (CBkListWnd*)FindChildByCmdID(IDC_PROC_LIST);
	SetTimer(TIMER_UPDATE_PROC_LIST, TIMER_UPDATE_PROC_LIST_SPAN);
	return TRUE;
}

void KProcListWnd::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_UPDATE_PROC_LIST)
	{
		UpdateProcList();
	}
}

void KProcListWnd::UpdateProcList()
{
	DWORD dwPids[1024];
	DWORD dwProcCount;

	DataCenter::GetInstance()->ClearProcInfos();

	GetProcIds(dwPids, 1024, &dwProcCount);
	for (int i = 0; i < dwProcCount; i++)
	{
		GetProcBaseInfo(dwPids[i], KProcListWnd::OnGetProcBaseInfo);
	}

	UpdateProcListUI();
}

void KProcListWnd::OnGetProcBaseInfo(LPCTSTR lpszProcName, DWORD dwPid, LPCTSTR lpszProcPath, DWORD dwCpuUse, DWORD dwMemoryUse)
{
	DataCenter* pDataCenter = DataCenter::GetInstance();
	ProcInfo* pInfo = pDataCenter->GetProcInfo(dwPid);
	if (NULL == pInfo)
	{
		pInfo = pDataCenter->CreateProcInfo();
		pInfo->m_dwPid = dwPid;
		pDataCenter->AddProcInfo(pInfo);
	}

	pInfo->m_strProcName = lpszProcName;
	pInfo->m_strProcPath = lpszProcPath;
	pInfo->m_dwCpuUse = dwCpuUse;
	pInfo->m_dwMemoryUse = dwMemoryUse;
}

void KProcListWnd::UpdateProcListUI()
{
	m_pListWnd->DeleteAllItem();

	std::vector< ProcInfo* >::iterator iter;
	std::vector< ProcInfo* > vecProcInfos;
	DataCenter::GetInstance()->GetAllProcInfos(vecProcInfos);
	for (iter = vecProcInfos.begin(); iter != vecProcInfos.end(); iter++)
	{
		CreateItemByProcInfo(*iter);
	}

	m_pListWnd->UpdateLayout(TRUE);
}

void KProcListWnd::CreateItemByProcInfo(ProcInfo* pInfo)
{
	CBkListItem* pListItem = new CBkListItem();
	pListItem->SetAttribute("height", "25", FALSE);

    BkWin::BkLinearLayout* pLayout = new BkWin::BkLinearLayout();
    pLayout->SetAttribute("pos", "0,0,-0,25", FALSE);

    CBkCheckBox* pCheckBox = new CBkCheckBox();
    pCheckBox->SetAttribute("pos", "0,0,20,20", FALSE);
    pCheckBox->SetAttribute("layout_margin_left", "5", FALSE);
    pCheckBox->SetAttribute("layout_margin_top", "5", FALSE);
    pLayout->AppendChild(pCheckBox);

	CBkStatic* pTextProcName = new CBkStatic();
	pTextProcName->SetAttribute("pos", "0,0,150,25", FALSE);
	pTextProcName->SetInnerText(pInfo->m_strProcName);
	pLayout->AppendChild(pTextProcName);

	CString strPid;
	strPid.Format(_T("%d"), pInfo->m_dwPid);
	CBkStatic* pTextPid = new CBkStatic();
    pTextPid->SetAttribute("id", "101", FALSE);
	pTextPid->SetAttribute("pos", "0,0,80,25", FALSE);
	pTextPid->SetInnerText(strPid);
	pLayout->AppendChild(pTextPid);

	CBkStatic* pTextProcPath = new CBkStatic();
    pTextProcPath->SetAttribute("id", "100", FALSE);
	pTextProcPath->SetAttribute("pos", "0,0,200,25", FALSE);
	pTextProcPath->SetInnerText(pInfo->m_strProcPath);
	pLayout->AppendChild(pTextProcPath);

	CString strCpuUse;
	strCpuUse.Format(_T("%d%%"), pInfo->m_dwCpuUse);
	CBkStatic* pTextCpuUse = new CBkStatic();
	pTextCpuUse->SetAttribute("pos", "0,0,80,25", FALSE);
	pTextCpuUse->SetInnerText(strCpuUse);
	pLayout->AppendChild(pTextCpuUse);

	CString strMemoryUse;
	strMemoryUse.Format(_T("%dK"), pInfo->m_dwMemoryUse);
	CBkStatic* pTextMemoryUse = new CBkStatic();
	pTextMemoryUse->SetAttribute("pos", "0,0,80,25", FALSE);
	pTextMemoryUse->SetInnerText(strMemoryUse);
	pLayout->AppendChild(pTextMemoryUse);

    pListItem->AddView(pLayout);
    pListItem->SetInnerText(strPid);

    //CBkLine* PLine = new CBkLine();
    //PLine->SetAttribute("pos", "0,-1,-0,-0", FALSE);
    //pListItem->AddView(PLine);

	m_pListWnd->AppendListItem(pListItem);
}

void KProcListWnd::OnListItemRBtnUp(int nListItem)
{
    CString strPath = GetItemInnerText(nListItem, IDC_PROCESS_PATH);
    CString strPid = GetItemInnerText(nListItem, IDC_PROCESS_ID);

    m_selectProInfo = ProcInfo();
    m_selectProInfo.m_strProcPath = strPath;
    m_selectProInfo.m_dwPid = _wtoi(strPid);

    ShowRightMenu();
}

LRESULT KProcListWnd::OnUserCommand(UINT uNotifyCode, UINT nID, HWND hWnd)
{
    switch(nID)
    {
    case IDM_KILL_PROCESS:
        CloseProc(m_selectProInfo.m_dwPid);
        break;
    case IDM_FILE_LOCATION:
        {
            CString strFolder = KDubaPath::PathToFolderPath(m_selectProInfo.m_strProcPath);
            ::ShellExecute(NULL, L"open", L"explorer.exe", strFolder, NULL, SW_SHOWNORMAL);
        }
        break;
    case IDM_FILE_ATTRIBUTE:
        OpenFileAttribute();
        break;
    case IDM_EXPORT_INFO:
        ExportInfo();
        break;
    default:
        break;
    }

    return 0;
}

void KProcListWnd::ShowRightMenu()
{
    CMenu menu;
    POINT pos;
    if (menu.CreatePopupMenu())
    {
        menu.AppendMenu(MF_STRING, IDM_KILL_PROCESS, L"结束进程");
        menu.AppendMenu(MF_STRING, IDM_FILE_LOCATION, L"定位文件");
        menu.AppendMenu(MF_STRING, IDM_FILE_ATTRIBUTE, L"文件属性");
        menu.AppendMenu(MF_STRING, IDM_EXPORT_INFO, L"导出信息");

        ::SetForegroundWindow(m_hWnd);
        ::GetCursorPos(&pos);
        menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, m_hWnd);		
    }
}

CString KProcListWnd::GetItemInnerText(int nListItem, int nItemChildId)
{
    CBkListItem* pItem = m_pListWnd->GetListItem(nListItem);
    if (NULL == pItem)
    {
        return L"";
    }

    CBkWindow* pProcessId = pItem->FindChildByCmdID(nItemChildId);
    if (NULL == pProcessId)
    {
        return L"";
    }

    return pProcessId->GetInnerText();
}

void KProcListWnd::OpenFileAttribute()
{
    SHELLEXECUTEINFO info = { 0 };
    info.cbSize = sizeof info;
    info.lpFile = m_selectProInfo.m_strProcPath;
    info.nShow = SW_SHOW;
    info.fMask = SEE_MASK_INVOKEIDLIST;
    info.lpVerb = L"properties";
    ShellExecuteEx(&info);
}

void KProcListWnd::ExportInfo()
{
    OPENFILENAME ofn;
    TCHAR szFile[MAX_PATH] = L"computerInfo.ini";
    ZeroMemory(&ofn,sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = _T("Ini\0*.ini\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
    GetSaveFileName(&ofn);

    CString strPath(ofn.lpstrFile);
    ExportProcInfo(m_selectProInfo.m_dwPid, strPath);
}
