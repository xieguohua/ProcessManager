//********************************************************************
//author:  hexujun@cmcm.com
//create time: 2020/6/4?15:29
//description: 支持菜单窗口, 通过xml配置,支持多级菜单展示, 菜单支持滚动条
// 用法：
//m_pTopMenu = new CBkDialogMenu();
//m_pTopMenu->SetCmdListener(this);
//m_pTopMenu->CreateMenu(m_hWnd, IDR_DLG_TOP_MENU); // menu_xml 资源id,参考：menu_xml
//m_pTopMenu->PopUp(pt);

// 由两个xml组成：
// dialog_layout_xml: 默认提供可以定制化
//<?xml version=\"1.0\" encoding=\"utf-8\"?>\
<layer width=\"20\" height=\"10\" appwin=\"0\" resize=\"0\" dpiaware=\"1\">\
	<body width=\"full\" height=\"full\">\
		<listwnd id=\"1\" pos=\"0,0,-0,-0\">\
			<listitem height=\"0\"/>\
		</listwnd>\
	</body>\
//</layer>"

// menu_xml:
/*<?xml version="1.0" encoding="utf-8"?>
<menuroot width="100" padding="10" crbg="000000">
	<menuitem name="menu_open_image" height="20" class="context_dlg_click_text">
		<text id="1003" pos="0,0,-40,-0" class="context_dlg_normal_text" transparent="1">打开图片1</text>
		<text id="1004" pos="-40,0,-0,-0" class="context_dlg_normal_text" transparent="1">Ctrl+O </text>
	</menuitem>
	<menuitem name="menu_open_image" height="20" class="context_dlg_click_text">
		<text id="1003" pos="0,0,-40,-0" class="context_dlg_normal_text" transparent="1">打开图片2</text>
		<text id="1004" pos="-40,0,-0,-0" class="context_dlg_normal_text" transparent="1">Ctrl+O </text>
	</menuitem>
	<menuitem name="menu_open_image" height="20" class="context_dlg_click_text">
		<text id="1003" pos="0,0,-40,-0" class="context_dlg_normal_text" transparent="1">打开图片3</text>
		<text id="1004" pos="-40,0,-0,-0" class="context_dlg_normal_text" transparent="1">Ctrl+O </text>
	</menuitem>
	<menuitem name="menu_open_image" height="20" class="context_dlg_click_text">
		<text id="1003" pos="0,0,-40,-0" class="context_dlg_normal_text" transparent="1">打开图片4</text>
		<text id="1004" pos="-40,0,-0,-0" class="context_dlg_normal_text" transparent="1">Ctrl+O </text>
		// 二级菜单
		<menuroot width="100" padding="10" crbg="000000">
			<menuitem name="menu_open_image" height="20" class="context_dlg_click_text">
				<text id="1003" pos="0,0,-40,-0" class="context_dlg_normal_text" transparent="1">打开图片1</text>
				<text id="1004" pos="-40,0,-0,-0" class="context_dlg_normal_text" transparent="1">Ctrl+O </text>
			</menuitem>
			<menuitem name="menu_open_image" height="20" class="context_dlg_click_text">
				<text id="1003" pos="0,0,-40,-0" class="context_dlg_normal_text" transparent="1">打开图片2</text>
				<text id="1004" pos="-40,0,-0,-0" class="context_dlg_normal_text" transparent="1">Ctrl+O </text>
			</menuitem>
			<menuitem name="menu_open_image" height="20" class="context_dlg_click_text">
				<text id="1003" pos="0,0,-40,-0" class="context_dlg_normal_text" transparent="1">打开图片3</text>
				<text id="1004" pos="-40,0,-0,-0" class="context_dlg_normal_text" transparent="1">Ctrl+O </text>
			</menuitem>
			<menuitem name="menu_open_image" height="20" class="context_dlg_click_text">
				<text id="1003" pos="0,0,-40,-0" class="context_dlg_normal_text" transparent="1">打开图片4</text>
				<text id="1004" pos="-40,0,-0,-0" class="context_dlg_normal_text" transparent="1">Ctrl+O </text>
			</menuitem>
		</menuroot>
	</menuitem>
</menuroot>*/
//*********************************************************************

#ifndef _H_CBKDIALOGMENU_H_
#define _H_CBKDIALOGMENU_H_

#include "CBkDialogViewImplEx.h"
#include <map>

class CBkMenuItem;
class CBkMenuRoot;
class CBkDialogMenu;

class CBkMenuItem : public CBkDialogButton 
{
public:
	BKOBJ_DECLARE_CLASS_NAME(CBkMenuItem, "menuitem")

public:
	CBkMenuItem() 
	{
		m_nItemHeight = 0;
		m_pSubMenuXmlChild = NULL;
	}

	~CBkMenuItem() 
	{
		if (m_pSubMenuXmlChild != NULL) 
		{
			delete m_pSubMenuXmlChild;
			m_pSubMenuXmlChild = NULL;
		}
	}

	TiXmlElement* GetSubMenuXmlElm() 
	{
		return m_pSubMenuXmlChild;
	}

	LPCTSTR GetMenuName() const 
	{
		return m_strMenuName;
	}

protected:

	BKWIN_DECLARE_ATTRIBUTES_BEGIN()
		BKWIN_CUSTOM_ATTRIBUTE("height", OnAttributeHeigthChange)
		BKWIN_CUSTOM_ATTRIBUTE("pos", OnAttributePosChange)
		BKWIN_TSTRING_ATTRIBUTE("name", m_strMenuName, FALSE)
	BKWIN_DECLARE_ATTRIBUTES_END()

	BKWIN_BEGIN_MSG_MAP()
		MSG_WM_NCCALCSIZE(OnNcCalcSize)
		MSG_WM_WINDOWPOSCHANGED(OnWindowPosChanged)
	BKWIN_END_MSG_MAP()

protected:

	void OnWindowPosChanged(LPWINDOWPOS lpWndPos)
	{
		m_rcWindow.MoveToXY(lpWndPos->x, lpWndPos->y);
		m_rcWindow.right = m_rcWindow.left + lpWndPos->cx;
		m_rcWindow.bottom = m_rcWindow.top + lpWndPos->cy;
		RepositionChilds();
	}

	HRESULT OnAttributeHeigthChange(CStringA& strValue, BOOL bLoading)
	{
		int nHeigth = atoi(strValue);
		nHeigth = ScaleByDpi(nHeigth);
		if (nHeigth >=0 && nHeigth != m_nItemHeight)
		{		 
			m_nItemHeight = nHeigth;
			if (!bLoading)
				_RepositionSelf();
		}

		return S_OK;
	}

	LRESULT OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam)
	{
		LPSIZE pSize = (LPSIZE)lParam;

		if (m_nItemHeight < 0)
		{
			m_nItemHeight = 0;
		}

		pSize->cx = 20;
		pSize->cy = m_nItemHeight;

		return S_OK;
	}


	virtual BOOL LoadChilds(TiXmlElement* pTiXmlChildElem)
	{
		BkSendMessage(WM_DESTROY);

		BOOL bVisible = IsVisible(TRUE);

		for (TiXmlElement* pXmlChild = pTiXmlChildElem; NULL != pXmlChild; pXmlChild = pXmlChild->NextSiblingElement())
		{
			if(strcmp(pXmlChild->Value(), "menuroot") == 0)
			{
				if (m_pSubMenuXmlChild == NULL) 
				{
					m_pSubMenuXmlChild = (TiXmlElement*)pXmlChild->Clone();
				}
				continue;
			}

			CBkWindow *pNewChildWindow = _CreateBkWindowByName(pXmlChild->Value());
			if (!pNewChildWindow)
				continue;

			pNewChildWindow->SetParent(m_hBkWnd);
			pNewChildWindow->SetContainer(m_hWndContainer);
			pNewChildWindow->Load(pXmlChild);

			m_lstWndChild.AddTail(pNewChildWindow);
		}

		return TRUE;
	}

protected:
	int m_nItemHeight;
	int m_nMaxHeight;
	TiXmlElement* m_pSubMenuXmlChild;
	CString m_strMenuName;
};


class CBkMenuRoot : public CBkDialog 
{
public:
	BKOBJ_DECLARE_CLASS_NAME(CBkMenuRoot, "menuroot")

public:
	CBkMenuRoot() 
	{
		m_nWidth = 0;
		m_nPadding = 0;
		m_nMaxHeight = 0x0FFFFFF;
	}

	int GetMaxHeight() const 
	{
		return m_nMaxHeight;
	}

public:

	virtual void RepositionChilds()
	{
		WINDOWPOS WndPos ={0};

		WndPos.x = m_rcWindow.left + m_nPadding;
		WndPos.cx = m_rcWindow.Width() - 2 * m_nPadding;
		WndPos.y = m_rcWindow.top + m_nPadding;

		POSITION pos = m_lstWndChild.GetHeadPosition();
		while (pos != NULL)
		{
			CBkWindow *pBkWndChild = m_lstWndChild.GetNext(pos);
			if (!pBkWndChild || !pBkWndChild->IsVisible(FALSE))
			{
				continue;
			}

			if(pBkWndChild->IsClass(CBkMenuItem::GetClassName())) 
			{
				SIZE  szGet  = {0};
				pBkWndChild->BkSendMessage(WM_NCCALCSIZE, TRUE, (LPARAM)&szGet);
				WndPos.cy = szGet.cy;
				pBkWndChild->BkSendMessage(WM_WINDOWPOSCHANGED, NULL, (LPARAM)&WndPos);

				WndPos.y += szGet.cy;
			}
			else {
				 RepositionChild(pBkWndChild);
			}
		}
	}

	virtual BOOL LoadChilds(TiXmlElement* pTiXmlChildElem)
	{
		BkSendMessage(WM_DESTROY);

		BOOL bVisible = IsVisible(TRUE);

		for (TiXmlElement* pXmlChild = pTiXmlChildElem; NULL != pXmlChild; pXmlChild = pXmlChild->NextSiblingElement())
		{
			AddMenuItemView(pXmlChild);
		}

		return TRUE;
	}

	CBkWindow* AddMenuItemView(TiXmlElement* pXmlChild) 
	{
		const bool isMenuItemClass = (strcmp(pXmlChild->Value(), CBkMenuItem::GetClassName()) == 0);
		CBkWindow *pNewChildWindow = NULL;
		if (isMenuItemClass) 
		{
			CBkMenuItem* pMenuItem = new CBkMenuItem();
			pMenuItem->SetExternCreator(m_pOutCreator);
			pNewChildWindow = pMenuItem;
		}
		else
		{
			pNewChildWindow = _CreateBkWindowByName(pXmlChild->Value());
		}

		if (!pNewChildWindow) 
		{
			return NULL;
		}

		pNewChildWindow->SetParent(m_hBkWnd);
		pNewChildWindow->SetContainer(m_hWndContainer);
		pNewChildWindow->Load(pXmlChild);
		if(isMenuItemClass) 
		{
			pNewChildWindow->SetAttribute("pos", "0,0,-0,-0", TRUE);
		}

		m_lstWndChild.AddTail(pNewChildWindow);

		return pNewChildWindow;
	}


protected:
	BKWIN_DECLARE_ATTRIBUTES_BEGIN()
		BKWIN_CUSTOM_ATTRIBUTE("width", OnAttributeWidthChange)
		BKWIN_CUSTOM_ATTRIBUTE("padding", OnAttributePaddingChange)
		BKWIN_CUSTOM_ATTRIBUTE("max_height", OnAttributeMaxHeigthChange)
	BKWIN_DECLARE_ATTRIBUTES_END()

	BKWIN_BEGIN_MSG_MAP()
		MSG_WM_NCCALCSIZE(OnNcCalcSize)
		MSG_WM_WINDOWPOSCHANGED(OnWindowPosChanged)
	BKWIN_END_MSG_MAP()

protected:

	void OnWindowPosChanged(LPWINDOWPOS lpWndPos)
	{
		CBkWindow::OnWindowPosChanged(lpWndPos);
		RepositionChilds();
	}

	LRESULT OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam)
	{
		LPSIZE pSize = (LPSIZE)lParam;

		CSize szRet(0, 0);
		SIZE  szGet  = {0};

		POSITION pos = m_lstWndChild.GetHeadPosition();

		while (pos != NULL)
		{
			CBkWindow *pBkWndChild = m_lstWndChild.GetNext(pos);
			if (pBkWndChild && pBkWndChild->IsVisible(FALSE) && pBkWndChild->IsClass(CBkMenuItem::GetClassName()))
			{
				pBkWndChild->BkSendMessage(WM_NCCALCSIZE, TRUE, (LPARAM)&szGet);
				szRet.cy += szGet.cy;
			}
		}

		pSize->cx = m_nWidth;
		pSize->cy = szRet.cy + 2 * m_nPadding;

		return S_OK;
	}

	HRESULT OnAttributeMaxHeigthChange(CStringA& strValue, BOOL bLoading)
	{
		int nHeigth = atoi(strValue);
		nHeigth = ScaleByDpi(nHeigth);
		if (nHeigth >=0 && nHeigth != m_nMaxHeight)
		{		 
			m_nMaxHeight = nHeigth;
			if (!bLoading)
				_RepositionSelf();
		}

		return S_OK;
	}

	HRESULT OnAttributeWidthChange(CStringA& strValue, BOOL bLoading)
	{
		int nWidth = atoi(strValue);
		nWidth = ScaleByDpi(nWidth);
		if (nWidth >=0 && nWidth != m_nWidth)
		{		 
			m_nWidth = nWidth;
			if (!bLoading)
				_RepositionSelf();
		}

		return S_OK;
	}

	HRESULT OnAttributePaddingChange(CStringA& strValue, BOOL bLoading)
	{
		int nWidth = atoi(strValue);
		nWidth = ScaleByDpi(nWidth);
		if (nWidth >=0 && nWidth != m_nWidth)
		{		 
			m_nPadding = nWidth;
		}

		return S_OK;
	}

protected:
	int m_nWidth;
	int m_nPadding;
	int m_nMaxHeight;
};



/**
* 分为布局xml， menu.xml
**/
#define CBKDIALOGMENU_LAYOUT_XML "<?xml version=\"1.0\" encoding=\"utf-8\"?>\
<layer width=\"20\" height=\"10\" appwin=\"0\" resize=\"0\" dpiaware=\"1\">\
	<body width=\"full\" height=\"full\">\
		<listwnd id=\"1\" pos=\"0,0,-0,-0\">\
			<listitem height=\"0\"/>\
		</listwnd>\
	</body>\
</layer>"


template<class CBkDialogMenuT>
class CBkDialogMenuImpl 
	: public CBkDialogViewImplEx<CBkDialogMenuT>
{
public:
	friend class CBkMenuItem;

	enum {
		LISTNWD_ID = 1,

		TIMER_ID_CHECK_INACTIVE = 1000000,
	};

	typedef CBkDialogMenuT THE_CLASS_T;

	class IBKMenuCmdListener 
	{
	public:
		virtual void OnBkMenuCmd(THE_CLASS_T*  pDialogMenu, LPCWSTR lpszMenuName) = 0;
		virtual bool OnInitPopUpSubMenu(THE_CLASS_T*  pDialogMenu, LPCWSTR lpszMenuName) {
			return true;
		};
	};

	CBkDialogMenuImpl(UINT uLayoutResID = 0) : CBkDialogViewImplEx<CBkDialogMenuT>(0)
	{
		m_pParentDialogMenu = NULL;
		m_pLastHoverDialogMenu = NULL;
		m_pMenuRoot = NULL;
		m_pIBKMenuCmdListener = NULL;
		m_uBeiginMenuItemID = 1000000;
		m_uNextItemID = m_uBeiginMenuItemID;
		m_pListItem = NULL;
		m_pListWnd = NULL;
		m_bActive = FALSE;
		SetDrawBorder(FALSE);
		EnableShadow(FALSE);

		CStringA strXml;
		if (uLayoutResID != 0) {
			BkResManager::LoadResource(uLayoutResID, strXml);
		}
		m_strLayerXml = strXml.IsEmpty() ? CBKDIALOGMENU_LAYOUT_XML : strXml;
	}

	void SetCmdListener(IBKMenuCmdListener* pIBKMenuCmdListener) 
	{
		m_pIBKMenuCmdListener = pIBKMenuCmdListener;
		for (std::map<CBkMenuItem*, THE_CLASS_T*>::iterator iter = m_mapChildsDialogMenu.begin();
			iter != m_mapChildsDialogMenu.end();
			++iter) 
		{
			THE_CLASS_T* pMenuWnd = iter->second;
			pMenuWnd->SetCmdListener(pIBKMenuCmdListener);
		}
	}

	BOOL CreateMenu(HWND hParent, CBkMenuRoot* pMenuRoot) 
	{
		if(!CreateMenuWnd(hParent)) 
		{
			return FALSE;
		}

		m_pMenuRoot = pMenuRoot;
		m_pListItem->AppendChild(m_pMenuRoot);
		CreateSubMenu(pMenuRoot);

		return TRUE;
	}

	BOOL CreateMenu(HWND hParent, UINT uMenuResID) 
	{
		if(!CreateMenuWnd(hParent)) 
		{
			return FALSE;
		}

		CBkMenuRoot* pMenuRoot = LoadToMenuRoot(uMenuResID);
		if(pMenuRoot == NULL) 
		{
			DestroyWindow();
			return FALSE;
		}

		if(!CreateMenu(hParent, pMenuRoot)) 
		{
			delete pMenuRoot;
			return FALSE;
		}

		return TRUE;
	}


	struct MenuSizeInfo 
	{
		CSize sizeWnd;
		CSize sizeMenuRoot;
	};

	MenuSizeInfo CalcSize() 
	{
		MenuSizeInfo sizeRetInfo;

		if (!Validate()) 
		{
			return sizeRetInfo;
		}

		SIZE  szGet  = {0};
		m_pMenuRoot->BkSendMessage(WM_NCCALCSIZE, TRUE, (LPARAM)&szGet);
		const int nMaxHeight = m_pMenuRoot->GetMaxHeight();

		sizeRetInfo.sizeMenuRoot = szGet;

		sizeRetInfo.sizeWnd.cx = szGet.cx;
		sizeRetInfo.sizeWnd.cy = (szGet.cy > nMaxHeight ? nMaxHeight : szGet.cy);

		return sizeRetInfo;
	}

	BOOL PopUp(const POINT& pt)
	{
		if(!IsWindow()) 
		{
			return FALSE;
		}

		if (!Validate()) 
		{
			return FALSE;
		}

		CRect rcOld;
		GetWindowRect(rcOld);

		MenuSizeInfo sizeRetInfo = CalcSize();

		CRect rcWindow;
		rcWindow.left = pt.x;
		rcWindow.top = pt.y;
		rcWindow.right = rcWindow.left + sizeRetInfo.sizeWnd.cx;
		rcWindow.bottom = rcWindow.top + sizeRetInfo.sizeWnd.cy;

		m_pListItem->SetItemHeightDirect(sizeRetInfo.sizeMenuRoot.cy);

		CRect rcScreen;
		if (IsRootDialogMenu() && GetScreenRectFromPoint(rcWindow.TopLeft(), rcScreen)) 
		{
			CRect rcWorkArea(0, 0, 0, 0);
			::SystemParametersInfo(SPI_GETWORKAREA, 0,  &rcWorkArea, 0);
			const int nTaskBarHeigth =  ::GetSystemMetrics(SM_CYSCREEN) - rcWorkArea.Height();

			rcScreen.bottom -= nTaskBarHeigth;
			rcScreen.right -= 10;
			if(rcWindow.Height() > rcScreen.Height() && rcScreen.Height() > 0) 
			{
				rcWindow.top = rcWindow.top + rcScreen.Height();
			}

			if (rcWindow.right > rcScreen.right) 
			{
				rcWindow.OffsetRect(rcScreen.right - rcWindow.right, 0);
			}

			if (rcWindow.bottom > rcScreen.bottom) 
			{
				rcWindow.OffsetRect(0, - rcWindow.Height() - 20);
			}
		}

		MoveWindow(rcWindow, TRUE);

		if (rcOld.Width() == rcWindow.Width() &&
			rcOld.Height() == rcWindow.Height()) 
		{
			_RepositionItems();
		}
		
		m_pListWnd->UpdateLayout(TRUE);
		m_bActive = true;

		if(!IsWindowVisible()) 
		{
			ShowWindow(SW_SHOW);
		}
		else 
		{
			BringWindowToTop();
			SetActiveWindow();
		}
		
		

		return TRUE;
	}

	bool Validate() const 
	{
		return m_pMenuRoot != NULL && m_pListItem != NULL && m_pListWnd != NULL;
	}

	CBkMenuRoot* GetMenuRoot() 
	{
		return m_pMenuRoot;
	} 

	//************************************************************************
	//* 获取二级、三级CBkDialogMenu对象，通过唯一的menuname                                                              
	//************************************************************************
	CBkDialogMenu* GetSubMenu(LPCWSTR lpszMenuName) 
	{
		for (std::map<CBkMenuItem*, THE_CLASS_T*>::iterator iter = m_mapChildsDialogMenu.begin();
			iter != m_mapChildsDialogMenu.end();
			++iter) 
		{
			CBkMenuItem* pMenuItem = iter->first;
			if (CString(lpszMenuName) == pMenuItem->GetMenuName()) 
			{
				return  iter->second;
			}
		}


		for (std::map<CBkMenuItem*, THE_CLASS_T*>::iterator iter = m_mapChildsDialogMenu.begin();
			iter != m_mapChildsDialogMenu.end();
			++iter) 
		{
			CBkDialogMenu* pMenu = iter->second->GetSubMenu(lpszMenuName);
			if (pMenu != NULL) 
			{
				return pMenu;
			}
		}

		return NULL;
	} 

	CBkMenuItem* GetMenuItem(LPCWSTR lpszMenuName) 
	{
		if(m_pMenuRoot == NULL) 
		{
			return NULL;
		}

		CAtlList<CBkWindow *>& listChilds = m_pMenuRoot->GetChilds();
		POSITION pos = listChilds.GetHeadPosition();

		while (pos != NULL)
		{
			CBkWindow *pBkWndChild = listChilds.GetNext(pos);
			if (pBkWndChild == NULL)
			{
				continue;
			}

			if (!pBkWndChild->IsClass(CBkMenuItem::GetClassName())) 
			{
				continue;
			}

			CBkMenuItem* pMenItem = (CBkMenuItem*)pBkWndChild;
			if (CString(lpszMenuName) == pMenItem->GetMenuName()) 
			{
				return pMenItem;
			}
		}

		for (std::map<CBkMenuItem*, THE_CLASS_T*>::iterator iter = m_mapChildsDialogMenu.begin();
			iter != m_mapChildsDialogMenu.end();
			++iter) 
		{
			THE_CLASS_T* pMenuWnd = iter->second;
			CBkMenuItem* pMenuItem = pMenuWnd->GetMenuItem(lpszMenuName);
			if (pMenuItem != NULL) 
			{
				return pMenuItem;
			}
		}

		return NULL;
	}

	CBkMenuItem* InsertMenuItem(UINT uMenuItemResID) 
	{
		CStringA strXml;
		BOOL bRet = BkResManager::LoadResource(uMenuItemResID, strXml);
		if (!bRet || strXml.IsEmpty()) 
		{
			return NULL;
		}

		return InsertMenuItem(strXml);
	}

	CBkMenuItem* InsertMenuItem(LPCSTR lszMenuItemXml) 
	{
		if (m_pMenuRoot == NULL) {
			return NULL;
		}

		TiXmlDocument xmlDoc;
		{ // Free stack
			xmlDoc.Parse(lszMenuItemXml, NULL, TIXML_ENCODING_UTF8);
		}

		if (xmlDoc.Error())
		{
			return NULL;
		}

		TiXmlElement *pXmlRootElem = xmlDoc.RootElement();
		if (pXmlRootElem == NULL) 
		{
			return NULL;
		}
		
		const bool isMenuItemClass = (strcmp(pXmlRootElem->Value(), CBkMenuItem::GetClassName()) == 0);
		if(!isMenuItemClass) {
			return NULL;
		}

		CBkWindow* pWnd = m_pMenuRoot->AddMenuItemView(pXmlRootElem);
		if (pWnd == NULL) {
			return NULL;
		}

		if(!pWnd->IsClass(CBkMenuItem::GetClassName())) {
			m_pMenuRoot->RemoveChildItem(pWnd, TRUE);
			return NULL;
		}
			
		CBkMenuItem* pMenuItem =  (CBkMenuItem*)pWnd;
		CreateSubMenuItem(pMenuItem);

		return pMenuItem;
	}

	void DeleteAllMenuItem() 
	{
		CAtlList<CBkWindow *>& listChilds = m_pMenuRoot->GetChilds();
		POSITION pos = listChilds.GetHeadPosition();

		std::vector<CBkWindow *> vecItems;
		while (pos != NULL)
		{
			CBkWindow *pBkWndChild = listChilds.GetNext(pos);
			if (pBkWndChild == NULL)
			{
				continue;
			}

			if (!pBkWndChild->IsClass(CBkMenuItem::GetClassName())) 
			{
				continue;
			}

			CBkMenuItem* pMenItem = (CBkMenuItem*)pBkWndChild;
			vecItems.push_back(pMenItem);
		}

		for (std::vector<CBkWindow *>::iterator iter = vecItems.begin();
			 iter != vecItems.end();
			 ++iter) 
		{
			CBkWindow* pWnd = *iter;
			POSITION pos = listChilds.Find(pWnd);
			if (pos != NULL) {
				listChilds.RemoveAt(pos);
			}

			pWnd->BkSendMessage(WM_DESTROY);
			delete pWnd;
		}

		for (std::map<CBkMenuItem*, THE_CLASS_T*>::iterator iter = m_mapChildsDialogMenu.begin();
			iter != m_mapChildsDialogMenu.end();
			++iter) 
		{
			THE_CLASS_T* pMenuWnd = iter->second;
			if (pMenuWnd->IsWindow()) 
			{
				pMenuWnd->DestroyWindow();
			}
			delete pMenuWnd;
		}

		m_mapChildsDialogMenu.clear();
	}

protected:


	CBkMenuRoot* LoadToMenuRoot(UINT uMenuResID)
	{
		CStringA strXml;
		BOOL bRet = BkResManager::LoadResource(uMenuResID, strXml);
		if (!bRet || strXml.IsEmpty()) 
		{
			return FALSE;
		}

		TiXmlDocument xmlDoc;
		{ // Free stack
			xmlDoc.Parse(strXml, NULL, TIXML_ENCODING_UTF8);
		}

		if (xmlDoc.Error())
		{
			return FALSE;
		}

		TiXmlElement *pXmlRootElem = xmlDoc.RootElement();
		if (pXmlRootElem == NULL) 
		{
			return FALSE;
		}

		if(strcmp(pXmlRootElem->Value(), CBkMenuRoot::GetClassName()) != 0)
		{
			return FALSE;
		}

		return CreateMenuRoot(pXmlRootElem);
	}

	CBkMenuRoot* CreateMenuRoot(TiXmlElement *pXmlRootElem) 
	{
		if(pXmlRootElem == NULL) 
		{
			return NULL;
		}

		CBkMenuRoot* pMenuRoot = new CBkMenuRoot();

		pMenuRoot->SetSupportDpiAware(m_bSupportDpiAware);
		pMenuRoot->SetExternCreator(&m_controCreator);
		pMenuRoot->SetAttribute("pos", "0,0,-0,-0", TRUE);
		pMenuRoot->Load(pXmlRootElem);

		return pMenuRoot;
	}

	BOOL CreateMenuWnd(HWND hParent) 
	{
		if(IsWindow()) 
		{
			return TRUE;
		}

		if(!SetXml(m_strLayerXml)) 
		{
			return FALSE;
		}

		HWND hWnd = CreateEx(hParent);
		if(hWnd == NULL)
		{
			return FALSE;
		}

		if (!InitListWnd()) 
		{
			DestroyWindow();
			return FALSE;
		}

		return TRUE;
	}

	void CreateSubMenu(CBkMenuRoot* pMenuRoot) 
	{
		if(pMenuRoot == NULL) 
		{
			return;
		}

		CAtlList<CBkWindow *>& listChilds = pMenuRoot->GetChilds();
		POSITION pos = listChilds.GetHeadPosition();

		while (pos != NULL)
		{
			CBkWindow *pBkWndChild = listChilds.GetNext(pos);
			if (pBkWndChild == NULL)
			{
				continue;
			}

			if (!pBkWndChild->IsClass(CBkMenuItem::GetClassName())) 
			{
				continue;
			}

			CBkMenuItem* pMenuItem = (CBkMenuItem*)pBkWndChild;
			CreateSubMenuItem(pMenuItem);
		}
	}

	void CreateSubMenuItem(CBkMenuItem* pMenuItem) 
	{
		if(pMenuItem == NULL) {
			return;
		}

		CStringA strCmdID;
		strCmdID.Format("%d", m_uNextItemID++);
		pMenuItem->SetAttribute("id", strCmdID, TRUE);

		CBkMenuRoot* pMenuRoot = CreateMenuRoot(pMenuItem->GetSubMenuXmlElm());
		if (pMenuRoot == NULL)
		{
			return;
		}

		THE_CLASS_T* pSubBkDialogMenu = new THE_CLASS_T();
		if (!pSubBkDialogMenu->CreateMenu(m_hWnd, pMenuRoot)) 
		{
			pSubBkDialogMenu->DestroyWindow();
			delete pSubBkDialogMenu;
			delete pMenuRoot;
		}
		else 
		{
			pSubBkDialogMenu->m_pParentDialogMenu = (THE_CLASS_T*)this;
			pSubBkDialogMenu->m_pIBKMenuCmdListener = m_pIBKMenuCmdListener;
			m_mapChildsDialogMenu[pMenuItem] = pSubBkDialogMenu;
		}
	}

	THE_CLASS_T* GetRootDialogMenu() 
	{
		if(m_pParentDialogMenu == NULL) 
		{
			return (THE_CLASS_T*)this;
		}

		return m_pParentDialogMenu->GetRootDialogMenu();
	}

	BOOL IsRootDialogMenu() const 
	{
		return m_pParentDialogMenu == NULL;
	}

	void HideMenu() 
	{
		if(!IsWindowVisible()) 
		{
			return;
		}

		m_pLastHoverDialogMenu = NULL;
		ShowWindow(SW_HIDE);
		for (std::map<CBkMenuItem*, THE_CLASS_T*>::iterator iter = m_mapChildsDialogMenu.begin();
			iter != m_mapChildsDialogMenu.end();
			++iter) 
		{
			THE_CLASS_T* pMenuWnd = iter->second;
			pMenuWnd->HideMenu();
		}
	}

	CRect GetBoundRect() 
	{
		CRect rcBound(0, 0, 0, 0);
		if (!Validate()) 
		{
			return rcBound;
		}

		GetWindowRect(rcBound);
		for (std::map<CBkMenuItem*, THE_CLASS_T*>::iterator iter = m_mapChildsDialogMenu.begin();
			iter != m_mapChildsDialogMenu.end();
			++iter) 
		{
			THE_CLASS_T* pMenuWnd = iter->second;
			if(!pMenuWnd->IsWindowVisible()) 
			{
				continue;
			}

			CRect rcTemp = pMenuWnd->GetBoundRect();
			rcBound.UnionRect(rcBound, rcTemp);
		}

		return rcBound;
	}

protected:
	THE_CLASS_T* m_pParentDialogMenu;
	std::map<CBkMenuItem*, THE_CLASS_T*> m_mapChildsDialogMenu; 
	CBkMenuRoot*  m_pMenuRoot;
	UINT m_uNextItemID;
	UINT m_uBeiginMenuItemID;
	IBKMenuCmdListener* m_pIBKMenuCmdListener;
	CStringA m_strLayerXml;
	THE_CLASS_T* m_pLastHoverDialogMenu;
	CBkListItem* m_pListItem;
	CBkListWnd* m_pListWnd;
	BOOL m_bActive;

protected:

	BK_NOTIFY_MAP(IDC_RICHVIEW_WIN_EX)
		BK_LISTWND_NOTIFY_BEGIN(LISTNWD_ID)//51为listwnd在xml中定义的id
			BK_LISTWND_LISTITEM_CHILD_LBUTTONUP(OnListItemChildLButtonUp)
			BK_LISTWND_LISTITEM_CHILD_MOUSEHOVER(OnListItemChildMouseHover)
		BK_LISTWND_NOTIFY_END()

// 		const UINT uItemID = ((LPBKNMCOMMAND)pnmh)->uItemID;
// 
// 		if(uItemID >= m_uBeiginMenuItemID) 
// 		{
// 			if (BKNM_COMMAND == uCode) 
// 			{
// 				OnClickMenuItem(uItemID);
// 				return TRUE; 
// 			}
// 
// 			if (BKNM_MOUSEHOVER == uCode) 
// 			{
// 				OnMouseHoverMenuItem(uItemID);
// 				return TRUE; 
// 			}
// 		}

    BK_NOTIFY_MAP_END()

	BEGIN_MSG_MAP_EX(CBkDialogMenuImpl<CBkDialogMenuT>)
		MSG_BK_NOTIFY(IDC_RICHVIEW_WIN_EX)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_NCACTIVATE(OnNcActivate)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_TIMER(OnTimer)
		CHAIN_MSG_MAP(CBkDialogViewImplEx<THE_CLASS_T>)
	END_MSG_MAP()

protected:
	void OnTimer(UINT_PTR nIDEvent) 
	{
		if(nIDEvent != TIMER_ID_CHECK_INACTIVE) 
		{
			SetMsgHandled(FALSE);
			return;
		}

		KillTimer(TIMER_ID_CHECK_INACTIVE);
		CheckInActive();
	}

	void OnLButtonDown(UINT nFlags, CPoint point) 
	{
		SetMsgHandled(FALSE);
		m_bActive = TRUE;
	}

	void OnLButtonUp(UINT nFlags, CPoint point) 
	{
		__super::OnLButtonUp(nFlags, point);
		if(m_pLastHoverDialogMenu != NULL) 
		{
			m_pLastHoverDialogMenu->SetActiveWindow();
		}
	}

	void OnShowWindow(BOOL bShow, UINT nStatus) 
	{
		if(!bShow) 
		{
			SwitchToInActive();
		}
	}

	BOOL OnNcActivate(BOOL bActive) 
	{
		if(!bActive) 
		{
			SwitchToInActive();
		}
		else 
		{
			m_bActive = TRUE;
		}

		return TRUE;
	}

	void OnDestroy() 
	{
		m_pMenuRoot = NULL;
		m_pListItem = NULL;
		m_pListWnd = NULL;

		SetMsgHandled(FALSE);
		for (std::map<CBkMenuItem*, THE_CLASS_T*>::iterator iter = m_mapChildsDialogMenu.begin();
			 iter != m_mapChildsDialogMenu.end();
			 ++iter) 
		{
THE_CLASS_T* pMenuWnd = iter->second;
			if(pMenuWnd->IsWindow()) {
				pMenuWnd->DestroyWindow();
			}

			delete pMenuWnd;
		}

		m_mapChildsDialogMenu.clear();
	}

	void OnListItemChildLButtonUp(int nListItem, UINT uCmdID) 
	{
		if(nListItem == 0) 
		{
			OnClickMenuItem(uCmdID);
		}
	}

	void OnListItemChildMouseHover(int nListItem, UINT uCmdID) 
	{
		if(nListItem == 0) 
		{
			OnMouseHoverMenuItem(uCmdID);
		}
	}

	void OnClickMenuItem(UINT uCmdID) 
	{
		if(!IsWindowVisible()) 
		{
			return;
		}

		CBkMenuItem* pMenuItem = FindMenuItemByCmdID(uCmdID);
		if (pMenuItem == NULL || m_mapChildsDialogMenu.find(pMenuItem) != m_mapChildsDialogMenu.end()) 
		{
			return;
		}

		GetRootDialogMenu()->HideMenu();
		if(m_pIBKMenuCmdListener != NULL) 
		{
			m_pIBKMenuCmdListener->OnBkMenuCmd(GetRootDialogMenu(), pMenuItem->GetMenuName());
		}
	}

	void OnMouseHoverMenuItem(UINT uCmdID) 
	{
		if(!IsWindowVisible()) 
		{
			return;
		}

		CBkMenuItem* pMenuItem = FindMenuItemByCmdID(uCmdID);
		if (pMenuItem == NULL || pMenuItem->IsDisabled()) 
		{
			return;
		}

		THE_CLASS_T* pHoverDialogMenu = NULL;
		std::map<CBkMenuItem*, THE_CLASS_T*>::iterator iter = m_mapChildsDialogMenu.find(pMenuItem);
		if (iter != m_mapChildsDialogMenu.end()) 
		{
			pHoverDialogMenu = iter->second;
		}

		if(m_pLastHoverDialogMenu == pHoverDialogMenu) 
		{
			return;
		}

		if(m_pLastHoverDialogMenu != NULL) 
		{
			m_bActive = TRUE;
			m_pLastHoverDialogMenu->HideMenu();
		}

		m_pLastHoverDialogMenu = pHoverDialogMenu;
		BOOL bSuccess = FALSE;
		if (pHoverDialogMenu) 
		{
			bool bInitSuccess = true;
			if(m_pIBKMenuCmdListener != NULL) {
				bInitSuccess = m_pIBKMenuCmdListener->OnInitPopUpSubMenu(GetRootDialogMenu(), pMenuItem->GetMenuName());
			}

			if(bInitSuccess) 
			{
				CRect rcRect;
				pMenuItem->GetRect(rcRect);
				ClientToScreen(rcRect);


				CPoint ptShow;
				ptShow.x = rcRect.right;
				ptShow.y = rcRect.top;

				CRect rcScreen;
				if (GetScreenRectFromPoint(rcRect.CenterPoint(), rcScreen)) 
				{
					CRect rcMenuBound = GetRootDialogMenu()->GetBoundRect();
					MenuSizeInfo sizeInfo = pHoverDialogMenu->CalcSize();
					if(ptShow.x + sizeInfo.sizeWnd.cx > rcScreen.right) 
					{
						ptShow.x = rcMenuBound.left - sizeInfo.sizeWnd.cx;
					}

					if (ptShow.y + sizeInfo.sizeWnd.cy > rcScreen.bottom) 
					{
						ptShow.y = rcScreen.bottom - sizeInfo.sizeWnd.cy;
					}
				}

				bSuccess = pHoverDialogMenu->PopUp(ptShow);
			}
		}

		if(!bSuccess) 
		{
			SetActiveWindow();
		}
	}

	CBkMenuItem* FindMenuItemByCmdID(UINT uCmdID)
	{
		if(m_pListItem == NULL) 
		{
			return NULL;
		}

		CBkWindow* pWindow = m_pListItem->FindChildByCmdID(uCmdID);
		if (pWindow == NULL) {
			return NULL;
		}

		if (!pWindow->IsClass(CBkMenuItem::GetClassName())) 
		{
			return NULL;
		}

		return (CBkMenuItem*)pWindow;
	}

	BOOL InitListWnd() 
	{
		if(m_pListItem != NULL) 
		{
			return TRUE;
		}

		CBkWindow* pWnd = FindChildByCmdID(LISTNWD_ID);
		if (pWnd == NULL || !pWnd->IsClass(CBkListWnd::GetClassName())) 
		{
			return FALSE;
		}

		CBkListWnd* pListWnd = (CBkListWnd*)pWnd;
		m_pListItem = pListWnd->GetListItem(0);
		if(m_pListItem == NULL) {
			return FALSE;
		}

		m_pListWnd = pListWnd;

		return TRUE;
	}

	void SwitchToInActive() 
	{
		if(!m_bActive) 
		{
			return;
		}

		m_bActive = FALSE;
		GetRootDialogMenu()->KillTimer(TIMER_ID_CHECK_INACTIVE);
		GetRootDialogMenu()->SetTimer(TIMER_ID_CHECK_INACTIVE, 100);
	}

	void CheckInActive() 
	{
		if(!IsWindowVisible()) 
		{
			return;
		}

		THE_CLASS_T* pRootDialogMenu = GetRootDialogMenu();
		if (!pRootDialogMenu->IsChainListActive()) 
		{
			pRootDialogMenu->HideMenu();
		}
	}

	bool IsChainListActive() 
	{
		if(m_bActive) 
		{
			return true;
		}

		for (std::map<CBkMenuItem*, THE_CLASS_T*>::iterator iter = m_mapChildsDialogMenu.begin();
			iter != m_mapChildsDialogMenu.end();
			++iter) 
		{
			if( iter->second->IsChainListActive()) 
			{
				return true;
			}
		}

		return false;
	}

	BOOL GetScreenRectFromPoint(const CPoint &pt, CRect &rcScreen)
	{
		BOOL bReturn = FALSE;
		BOOL bRetCode = FALSE;
		int nNumber= GetSystemMetrics(SM_CMONITORS);
		CRect rcWork;

		for (int i = 0; i < nNumber; ++i)
		{
			DISPLAY_DEVICE dd;
			ZeroMemory(&dd, sizeof(dd));
			dd.cb = sizeof(dd);

			DEVMODE devMode;
			ZeroMemory(&devMode,sizeof(devMode));
			devMode.dmSize=sizeof(devMode);

			bRetCode = EnumDisplayDevices(NULL,i,&dd,0);
			if(!bRetCode) 
			{
				return FALSE;
			}

			bRetCode = EnumDisplaySettings(dd.DeviceName,ENUM_CURRENT_SETTINGS,&devMode);
			if(!bRetCode) 
			{
				return FALSE;
			}

			rcWork.left = devMode.dmPosition.x;
			rcWork.top = devMode.dmPosition.y;
			rcWork.right = rcWork.left + devMode.dmPelsWidth;
			rcWork.bottom = rcWork.top + devMode.dmPelsHeight;

			if (rcWork.PtInRect(pt))
			{
				rcScreen = rcWork;
				bReturn = TRUE;
				break;
			}
		}

		return bReturn;
	}
};

class CBkDialogMenu 
	: public CBkDialogMenuImpl<CBkDialogMenu> 
{
public:
	CBkDialogMenu(UINT uLayoutResID = 0) : CBkDialogMenuImpl<CBkDialogMenu>(uLayoutResID)
	{}
};


/**
 * 悬浮菜单，hover level后自动消失
 */
class CBkDialogHoverMenu 
	: public CBkDialogMenu
{
public:
	CBkDialogHoverMenu(UINT uLayoutResID = 0) : CBkDialogMenu(uLayoutResID)
	{
		m_bIsHover = false;
	}
public:

	 BEGIN_MSG_MAP_EX(CBkDialogMenu)
		 MSG_WM_MOUSEMOVE(OnMouseMove)
		 MSG_WM_MOUSELEAVE(OnMouseLeave)
		 CHAIN_MSG_MAP(CBkDialogMenuImpl<CBkDialogMenu>)
	END_MSG_MAP()

public:

	void Hide()
	{
		HideMenu();
	}

	bool IsHover()
	{
		return m_bIsHover;
	}

	void OnMouseMove(UINT nFlags, CPoint point) 
	{
		SetMsgHandled(FALSE);
		m_bIsHover = true;
	}

	void OnMouseLeave() 
	{
 		SetMsgHandled(FALSE);
		ShowWindow(SW_HIDE);
		m_bIsHover = false;
	}

private:
	bool m_bIsHover;
};

#endif