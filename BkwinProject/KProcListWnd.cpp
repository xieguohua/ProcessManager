#include "StdAfx.h"
#include "KProcListWnd.h"
#include "ProcInfoMgr/ProcInfoMgr.h"

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
	ProcInfo* pInfo = pDataCenter->CreateProcInfo();
	pInfo->m_strProcName = lpszProcName;
	pInfo->m_dwPid = dwPid;
	pInfo->m_strProcPath = lpszProcPath;
	pInfo->m_dwCpuUse = dwCpuUse;
	pInfo->m_dwMemoryUse = dwMemoryUse;

	pDataCenter->AddProcInfo(pInfo);
}

void KProcListWnd::UpdateProcListUI()
{
	m_pListWnd->DeleteAllItem();

	std::vector< ProcInfo* >::iterator iter;
	std::vector< ProcInfo* > & vecProcInfos = DataCenter::GetInstance()->GetAllProcInfos();
	for (iter = vecProcInfos.begin(); iter != vecProcInfos.end(); iter++)
	{
		CreateItemByProcInfo(*iter);
	}

	m_pListWnd->UpdateLayout(TRUE);
}

void KProcListWnd::CreateItemByProcInfo(ProcInfo* pInfo)
{
	CBkListItem* pListItem = new CBkListItem();
	pListItem->SetAttribute("height", "20", FALSE);

	CBkStatic* pTextProcName = new CBkStatic();
	pTextProcName->SetAttribute("pos", "0,0,80,20", FALSE);
	pTextProcName->SetInnerText(pInfo->m_strProcName);
	pListItem->AddView(pTextProcName);

	CString strPid;
	strPid.Format(_T("%d"), pInfo->m_dwPid);
	CBkStatic* pTextPid = new CBkStatic();
	pTextPid->SetAttribute("pos", "80,0,160,20", FALSE);
	pTextPid->SetInnerText(strPid);
	pListItem->AddView(pTextPid);

	CBkStatic* pTextProcPath = new CBkStatic();
	pTextProcPath->SetAttribute("pos", "160,0,240,20", FALSE);
	pTextProcPath->SetInnerText(pInfo->m_strProcPath);
	pListItem->AddView(pTextProcPath);

	m_pListWnd->AppendListItem(pListItem);
}
