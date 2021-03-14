#pragma once

#include "bkwin/CBkDialogViewImplEx.h"
#include "DataCenter.h"

class KProcListWnd: public CBkDialogViewImplEx<KProcListWnd>
{
public:
	enum
	{
		IDC_PROC_LIST = 2012,

        IDC_PROCESS_PATH = 100,
        IDC_PROCESS_ID = 101,
	};
	
	enum 
	{
		TIMER_UPDATE_PROC_LIST = 100,
		TIMER_UPDATE_PROC_LIST_SPAN = 1000,
	};

	KProcListWnd(void);
	~KProcListWnd(void);

protected:
	BK_NOTIFY_MAP(IDC_RICHVIEW_WIN_EX)
        BK_LISTWND_NOTIFY_BEGIN(IDC_PROC_LIST)
            BK_LISTWND_LISTITEM_RBUTTONUP(OnListItemRBtnUp)
        BK_LISTWND_NOTIFY_END()
	BK_NOTIFY_MAP_END()

	BEGIN_MSG_MAP_EX(CBkDialogViewImplEx<KProcListWnd>)
		MSG_BK_NOTIFY(IDC_RICHVIEW_WIN_EX)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_TIMER(OnTimer)
		CHAIN_MSG_MAP(CBkDialogViewImplEx<KProcListWnd>)
        MSG_WM_COMMAND(OnUserCommand)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/);
	void OnTimer(UINT_PTR nIDEvent);
    void OnListItemRBtnUp(int nListItem);
    LRESULT OnUserCommand(UINT uNotifyCode, UINT nID, HWND hWnd);

private:
	void UpdateProcList();
	static void OnGetProcBaseInfo(LPCTSTR lpszProcName, DWORD dwPid, LPCTSTR lpszProcPath, DWORD dwCpuUse, DWORD dwMemoryUse);
	void UpdateProcListUI();
	void CreateItemByProcInfo(ProcInfo* pInfo);
    void ShowRightMenu();
    CString GetItemInnerText(int nListItem, int nItemChildId);
    void OpenFileAttribute();
    void ExportInfo();

private:
	CBkListWnd* m_pListWnd;
    ProcInfo m_selectProInfo;
};
