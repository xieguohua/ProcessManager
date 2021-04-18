#pragma once

#include "bkwin/CBkDialogViewImplEx.h"
#include "DataCenter.h"
#include "SettingDefine.h"
#include <map>

class ProcInfo;
class KProcListWnd: public CBkDialogViewImplEx<KProcListWnd>
{

public:
	enum
	{
		IDC_PROC_LIST = 2012,
   
        IDC_PROC_PID = 2002,
        IDC_TITLE_START = 2003,
        IDC_PROC_PARENT_ID = 2003,
        IDC_PROC_SESSION_ID,
        IDC_PROC_CPU_USAGE,
        IDC_PROC_MEMORY_USAGE,
        IDC_PROC_HANDLE_COUND,
        IDC_PROC_CREATE_TIME,
        IDC_PROC_USER_COUNT,
        IDC_PROC_GDI_COUNT,
        IDC_PROC_PATH,
        IDC_TITLE_END,

        IDC_ITEM_CHECKED = 99,
        //IDC_ITEM_PROCESS_ID = 100,
        //IDC_ITEM_PARENT_ID,
        //IDC_ITEM_SESSION_ID,
        //IDC_ITEM_CPU_USAGE,
        //IDC_ITEM_MEMORY_USAGE,
        //IDC_ITEM_HANDLE_COUNT,
        //IDC_ITEM_CREATE_TIME,
        //IDC_ITEM_USER_COUNT,
        //IDC_ITEM_GDI_COUNT,
        //IDC_ITEM_PROCESS_PATH,
	};
	
	enum 
	{
        //timer
		TIMER_UPDATE_PROC_LIST = 100,

        //message
        UM_MSG_UPDATE_LIST = WM_USER + 1000,
	};

	KProcListWnd(void);
	~KProcListWnd(void);
    void UpdateSetting();
    void GetCheckedPrococess(std::vector<DWORD>& processIdList);

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
        MSG_WM_DESTROY(OnDestroy)
		CHAIN_MSG_MAP(CBkDialogViewImplEx<KProcListWnd>)
        MSG_WM_COMMAND(OnUserCommand)
        MESSAGE_HANDLER_EX(UM_MSG_UPDATE_LIST, OnCustomMsg)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

	BOOL OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/);
	void OnTimer(UINT_PTR nIDEvent);
    void OnDestroy();
    void OnListItemRBtnUp(int nListItem);
    LRESULT OnUserCommand(UINT uNotifyCode, UINT nID, HWND hWnd);
    LRESULT OnCustomMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    static void OnGetProcBaseInfo(ProcInfo* pInfo);
    static UINT WINAPI GetProcessInfoAsyn(LPVOID lpParam);
    static void UpdateProcList(KProcListWnd& procListWnd);
    void InitWorkThread();
    void ExitWorkThread();
	void UpdateProcListUI();
	CBkListItem* CreateItemByProcInfo(ProcInfo* pInfo);
    CBkStatic* CreateTextView(const CString& strInnerText, int nViewId);
    CBkStatic* CreateTextView(int nInnerText, int nViewId);
    void UpdateProcInfo(ProcInfo* pInfo);
    void ShowRightMenu();
    CString GetItemInnerText(int nListItem, int nItemChildId);
    void SetInnerText(CBkWindow* pView, const CString& strText);
    void SetInnerText(CBkWindow* pView, int nValue);
    void OpenFileAttribute();
    void ExportInfo();
    BOOL IsNeedShow(int nTitleId);
    BOOL IsEnableSetting(ENUM_SETTING settingOption);
    BOOL HasProcess(std::vector< ProcInfo* >& procInfos, int pid);
    void SetItemInfoVisible(int nViewId, BOOL bVisible);

private:
	CBkListWnd* m_pListWnd;
    ProcInfo m_selectProInfo;
    std::map<DWORD, CBkListItem*> m_itemMap;
    HANDLE  m_hWorkThread;
    HANDLE  m_hExitEvent;
    BOOL m_bInitUI;
};
