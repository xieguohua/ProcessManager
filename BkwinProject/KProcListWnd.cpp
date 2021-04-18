#include "StdAfx.h"
#include "KProcListWnd.h"
#include "ProcInfoMgr/ProcInfoMgr.h"
#include "bkwin/BkLinearLayout.h"
#include <shellapi.h>
#include "framework/KDubaPath.h"
#include <objbase.h>
#include <shlobj.h>
#include "KDlgSetting.h"

namespace
{
    const int IDM_KILL_PROCESS = 100;
    const int IDM_FILE_LOCATION = 101;
    const int IDM_FILE_ATTRIBUTE = 102;
    const int IDM_EXPORT_INFO = 103;

    const int UPDATE_PROC_LIST_SPAN = 2000;
    const int EXIT_TIME_OUT = 3000;
}

KProcListWnd::KProcListWnd(void)
		: CBkDialogViewImplEx<KProcListWnd>(IDR_PROC_LIST_WND)
		, m_pListWnd(NULL)
        , m_hWorkThread(NULL)
        , m_hExitEvent(NULL)
        , m_bInitUI(FALSE)
{
}

KProcListWnd::~KProcListWnd(void)
{
}

BOOL KProcListWnd::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_pListWnd = (CBkListWnd*)FindChildByCmdID(IDC_PROC_LIST);
    InitWorkThread();
    UpdateSetting();
	return TRUE;
}

void KProcListWnd::InitWorkThread()
{
    m_hExitEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hWorkThread = (HANDLE)_beginthreadex(NULL, 0, GetProcessInfoAsyn, this, 0, NULL);
}

void KProcListWnd::ExitWorkThread()
{
    if (NULL != m_hExitEvent)
    {
        if (NULL != m_hWorkThread)
        {
            ::SetEvent(m_hExitEvent);
            ::WaitForSingleObject(m_hWorkThread, EXIT_TIME_OUT);
            ::CloseHandle(m_hWorkThread);
            m_hWorkThread = NULL;
        }

        ::CloseHandle(m_hExitEvent);
        m_hExitEvent = NULL;
    }
}

UINT WINAPI KProcListWnd::GetProcessInfoAsyn(LPVOID lpParam)
{
    KProcListWnd* pThis = static_cast<KProcListWnd*>(lpParam);
    if (pThis == NULL)
    {
        return -1;
    }

    while (TRUE)
    {
        DWORD dwRet = WaitForSingleObject(pThis->m_hExitEvent, UPDATE_PROC_LIST_SPAN);
        if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_FAILED)
        {
            break;
        }
        else if (dwRet == WAIT_TIMEOUT)
        {
            UpdateProcList(*pThis);
        }
    }
    return 0;
}

void KProcListWnd::OnTimer(UINT_PTR nIDEvent)
{

}

void KProcListWnd::UpdateProcList(KProcListWnd& procListWnd)
{
	DWORD dwPids[1024];
	DWORD dwProcCount;

	DataCenter::GetInstance()->ClearProcInfos();

	GetProcIds(dwPids, 1024, &dwProcCount);
	for (int i = 0; i < dwProcCount; i++)
	{
		GetProcBaseInfo(dwPids[i], KProcListWnd::OnGetProcBaseInfo);
	}

    procListWnd.PostMessage(UM_MSG_UPDATE_LIST);
}

void KProcListWnd::OnGetProcBaseInfo(ProcInfo* pNewInfo)
{
    if (NULL == pNewInfo)
    {
        return;
    }

    DataCenter* pDataCenter = DataCenter::GetInstance();
    ProcInfo* pInfo = pDataCenter->GetProcInfo(pNewInfo->m_dwPid);
    if (NULL == pInfo)
    {
        pInfo = pDataCenter->CreateProcInfo();
        pInfo->m_dwPid = pNewInfo->m_dwPid;
        pDataCenter->AddProcInfo(pInfo);
    }

    pInfo->m_dwPid = pNewInfo->m_dwPid;
    pInfo->m_dwParentPid = pNewInfo->m_dwParentPid;
    pInfo->m_strCreateTime = pNewInfo->m_strCreateTime;
    pInfo->m_strProcName = pNewInfo->m_strProcName;
    pInfo->m_strProcPath = pNewInfo->m_strProcPath;
    pInfo->m_dwCpuUse = pNewInfo->m_dwCpuUse;
    pInfo->m_dwMemoryUse = pNewInfo->m_dwMemoryUse;
    pInfo->m_dwHandleCount = pNewInfo->m_dwHandleCount;
    pInfo->m_dwSessionId = pNewInfo->m_dwSessionId;
    pInfo->m_dwUserObjectCount = pNewInfo->m_dwUserObjectCount;
    pInfo->m_dwGDIObjectCount = pNewInfo->m_dwGDIObjectCount;
}

void KProcListWnd::UpdateProcListUI()
{
    //create or update item
	std::vector< ProcInfo* > curProcInfos;
	DataCenter::GetInstance()->GetAllProcInfos(curProcInfos);
	for (std::vector< ProcInfo* >::iterator iter = curProcInfos.begin(); iter != curProcInfos.end(); iter++)
	{
        ProcInfo* pInfo = *iter;
        std::map<DWORD, CBkListItem*>::iterator mapIter = m_itemMap.find(pInfo->m_dwPid);
        if (mapIter == m_itemMap.end() || NULL == mapIter->second)
        {
            CBkListItem* pItem = CreateItemByProcInfo(pInfo);
            m_pListWnd->AppendListItem(pItem);
            m_itemMap[pInfo->m_dwPid] = pItem;
        }
        else
        {
            UpdateProcInfo(pInfo);
        }
	}

    //delete item
    std::map<DWORD, CBkListItem*>::iterator mapIter = m_itemMap.begin();
    while (mapIter != m_itemMap.end())
    {
        DWORD dwPid = mapIter->first;
        CBkListItem* item = mapIter->second;
        if (!HasProcess(curProcInfos, dwPid) && NULL != item)
        {
            int nIndex = m_pListWnd->GetItemIndexByBkWnd(item->GetBkHWnd());
            m_pListWnd->DeleteItem(nIndex);
            m_itemMap[dwPid] = NULL;
        }
        
        mapIter++;
    }

    if (!m_bInitUI)
    {
        UpdateSetting();
        m_bInitUI = TRUE;
    }
}

CBkListItem* KProcListWnd::CreateItemByProcInfo(ProcInfo* pInfo)
{
    CStringA strItemId;
    CString strPid;
	CBkListItem* pListItem = new CBkListItem();
    strPid.Format(L"%d", pInfo->m_dwPid);
    pListItem->SetInnerText(strPid);
	pListItem->SetAttribute("height", "25", FALSE);

    BkWin::BkLinearLayout* pLayout = new BkWin::BkLinearLayout();
    pLayout->SetAttribute("pos", "0,0,-0,25", FALSE);

    CBkCheckBox* pCheckBox = new CBkCheckBox();
    strItemId.Format("%d", IDC_ITEM_CHECKED);
    pCheckBox->SetAttribute("id", strItemId, FALSE);
    pCheckBox->SetAttribute("pos", "0,0,20,20", FALSE);
    pCheckBox->SetAttribute("layout_margin_left", "5", FALSE);
    pCheckBox->SetAttribute("layout_margin_top", "5", FALSE);
    pLayout->AppendChild(pCheckBox);

	CBkStatic* pTextProcName = new CBkStatic();
	pTextProcName->SetAttribute("pos", "0,0,150,25", FALSE);
    pTextProcName->SetAttribute("class", "end_ellipsis", FALSE);
	pTextProcName->SetInnerText(pInfo->m_strProcName);
	pLayout->AppendChild(pTextProcName);

    pLayout->AppendChild(CreateTextView(pInfo->m_dwPid, IDC_PROC_PID));
    pLayout->AppendChild(CreateTextView(pInfo->m_dwParentPid, IDC_PROC_PARENT_ID));
    pLayout->AppendChild(CreateTextView(pInfo->m_dwSessionId, IDC_PROC_SESSION_ID));

    CString strCpuUse;
    strCpuUse.Format(_T("%d%%"), pInfo->m_dwCpuUse);
    pLayout->AppendChild(CreateTextView(strCpuUse, IDC_PROC_CPU_USAGE));

    CString strMemoryUse;
    strMemoryUse.Format(_T("%dK"), pInfo->m_dwMemoryUse);
    pLayout->AppendChild(CreateTextView(strMemoryUse, IDC_PROC_MEMORY_USAGE));

    pLayout->AppendChild(CreateTextView(pInfo->m_dwHandleCount, IDC_PROC_HANDLE_COUND));
    pLayout->AppendChild(CreateTextView(pInfo->m_strCreateTime, IDC_PROC_CREATE_TIME));
    pLayout->AppendChild(CreateTextView(pInfo->m_dwUserObjectCount, IDC_PROC_USER_COUNT));
    pLayout->AppendChild(CreateTextView(pInfo->m_dwGDIObjectCount, IDC_PROC_GDI_COUNT));

    CBkStatic* pTextProcPath = CreateTextView(pInfo->m_strProcPath, IDC_PROC_PATH);
    pTextProcPath->SetAttribute("pos", "0,0,300,25", FALSE);
    pTextProcPath->SetAttribute("class", "end_ellipsis", FALSE);
    pLayout->AppendChild(pTextProcPath);
    
    pListItem->AddView(pLayout);

    return pListItem;
}

CBkStatic* KProcListWnd::CreateTextView(const CString& strInnerText, int nViewId)
{
    CBkStatic* pTextView = new CBkStatic();
    CStringA strViewId;
    strViewId.Format("%d", nViewId);
    pTextView->SetAttribute("id", strViewId, FALSE);
    pTextView->SetAttribute("pos", "0,0,80,25", FALSE);
    pTextView->SetInnerText(strInnerText);
    return pTextView;
}

CBkStatic* KProcListWnd::CreateTextView(int nInnerText, int nViewId)
{
    CString strText;
    strText.Format(L"%d", nInnerText);
    return CreateTextView(strText, nViewId);
}

void KProcListWnd::OnListItemRBtnUp(int nListItem)
{
    CString strPath = GetItemInnerText(nListItem, IDC_PROC_PATH);
    CString strPid = GetItemInnerText(nListItem, IDC_PROC_PID);

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

    CBkWindow* pView = pItem->FindChildByCmdID(nItemChildId);
    if (NULL == pView)
    {
        return L"";
    }

    return pView->GetInnerText();
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
    ExportAllProcInfo(strPath);
}

void KProcListWnd::UpdateSetting()
{
    for (int nViewId = IDC_TITLE_START; nViewId < IDC_TITLE_END; ++nViewId)
    {
        BOOL bVisible = IsNeedShow(nViewId);
        SetItemVisible(nViewId, bVisible);
        SetItemInfoVisible(nViewId, bVisible);
    }
}

BOOL KProcListWnd::IsEnableSetting(ENUM_SETTING settingOption)
{
    KDlgSetting dlg;
    std::vector<ENUM_SETTING> vec = dlg.GetSettingOption();
    for (int nIndex = 0; nIndex < vec.size(); ++nIndex)
    {
        if (vec[nIndex] == settingOption)
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL KProcListWnd::HasProcess(std::vector< ProcInfo* >& procInfos, int pid)
{
    for (std::vector< ProcInfo* >::iterator iter = procInfos.begin(); iter != procInfos.end(); iter++)
    {
        ProcInfo* info = *iter;
        if (info->m_dwPid == pid)
        {
            return TRUE;
        }
    }

    return FALSE;
}

void KProcListWnd::UpdateProcInfo(ProcInfo* pInfo)
{
    if (NULL == pInfo)
    {
        return;
    }

    std::map<DWORD, CBkListItem*>::iterator mapIter = m_itemMap.find(pInfo->m_dwPid);
    if (mapIter == m_itemMap.end())
    {
        return;
    }

    CBkListItem* pItem = mapIter->second;
    if (NULL == pItem)
    {
        return;
    }

    SetInnerText(pItem->FindChildByCmdID(IDC_PROC_CPU_USAGE), pInfo->m_dwCpuUse);
    SetInnerText(pItem->FindChildByCmdID(IDC_PROC_MEMORY_USAGE), pInfo->m_dwMemoryUse);
    SetInnerText(pItem->FindChildByCmdID(IDC_PROC_HANDLE_COUND), pInfo->m_dwHandleCount);
    SetInnerText(pItem->FindChildByCmdID(IDC_PROC_USER_COUNT), pInfo->m_dwUserObjectCount);
    SetInnerText(pItem->FindChildByCmdID(IDC_PROC_GDI_COUNT), pInfo->m_dwGDIObjectCount);
}

void KProcListWnd::SetInnerText(CBkWindow* pView, const CString& strText)
{
    if (NULL != pView)
    {
        pView->SetInnerText(strText);
    }
}

void KProcListWnd::SetInnerText(CBkWindow* pView, int nValue)
{
    CString strText;
    strText.Format(L"%d", nValue);
    SetInnerText(pView, strText);
}

void KProcListWnd::GetCheckedPrococess(std::vector<DWORD>& processIdList)
{
    std::map<DWORD, CBkListItem*>::iterator iter;
    for(iter = m_itemMap.begin(); iter != m_itemMap.end(); iter++)
    {
        CBkListItem* pItem = iter->second;
        if (NULL == pItem)
        {
            continue;
        }

        CBkWindow* pCheckView = pItem->FindChildByCmdID(IDC_ITEM_CHECKED);
        if (NULL == pCheckView)
        {
            continue;
        }

        if (pCheckView->IsChecked())
        {
            processIdList.push_back(iter->first);
        }
    }
}

LRESULT KProcListWnd::OnCustomMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case UM_MSG_UPDATE_LIST:
        UpdateProcListUI();
        break;
    default:
        break;
    }

    return S_OK;
}

void KProcListWnd::OnDestroy()
{
    ExitWorkThread();
}

BOOL KProcListWnd::IsNeedShow(int nTitleId)
{
    BOOL bRet = FALSE;
    KDlgSetting dlg;
    std::vector<ENUM_SETTING> vec = dlg.GetSettingOption();
    for (int nIndex = 0; nIndex < vec.size(); ++nIndex)
    {
        ENUM_SETTING settingOption = vec[nIndex];
        int nViewId = 0;
        switch (settingOption)
        {
        case SETTING_CPU_USAGE:
            nViewId = IDC_PROC_CPU_USAGE;
            break;
        case SETTING_MEMORY_USAGE:
            nViewId = IDC_PROC_MEMORY_USAGE;
            break;
        case SETTING_PROCESS_PATH:
            nViewId = IDC_PROC_PATH;
            break;
        case SETTING_HANDLE_COUND:
            nViewId = IDC_PROC_HANDLE_COUND;
            break;
        case SETTING_PARENT_ID:
            nViewId = IDC_PROC_PARENT_ID;
            break;
        case SETTING_SESSION_ID:
            nViewId = IDC_PROC_SESSION_ID;
            break;
        case SETTING_GDI_COUNT:
            nViewId = IDC_PROC_GDI_COUNT;
            break;
        case SETTING_USER_COUNT:
            nViewId = IDC_PROC_USER_COUNT;
            break;
        case SETTING_CREATE_TIME:
            nViewId = IDC_PROC_CREATE_TIME;
            break;
        default:
            break;
        }

        if (nTitleId == nViewId)
        {
            bRet = TRUE;
        }
    }

    return bRet;
}

void KProcListWnd::SetItemInfoVisible(int nViewId, BOOL bVisible)
{
    std::map<DWORD, CBkListItem*>::iterator iter;
    for(iter = m_itemMap.begin(); iter != m_itemMap.end(); iter++)
    {
        CBkListItem* pItem = iter->second;
        if (NULL == pItem)
        {
            continue;
        }

        CBkWindow* pView = pItem->FindChildByCmdID(nViewId);
        if (NULL != pView)
        {
            pView->SetVisible(bVisible);
        }
    }
}
