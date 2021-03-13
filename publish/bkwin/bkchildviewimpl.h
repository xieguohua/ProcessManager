/*************************************************
//  File name:       bkchildviewimpl.h
//  Date:              2020/11/18
//  <author>      hexujun@cmcm.com
//  <time>          2020/11/18
//   <version>     1.0    
*************************************************/
#ifndef _bkchildviewimpl_h_
#define _bkchildviewimpl_h_

template <class T>
class  BkChildViewImpl
{
public:
	BkChildViewImpl() 
	{

	}

	BOOL SetRichText(UINT uItemID, UINT uResID)
	{
		CStringA strXml;
		BOOL bRet = BkResManager::LoadResource(uResID, strXml);
		if (!bRet)
			return FALSE;

		return SetRichText(uItemID, strXml);
	}

	BOOL SetRichText(UINT uItemID, LPCWSTR lpszXml)
	{
		CStringA strXml = CW2A(lpszXml, CP_UTF8);

		return SetRichText(uItemID, strXml);
	}

	BOOL SetRichText(UINT uItemID, LPCSTR lpszXml)
	{
		T* pT = static_cast<T*>(this);
		CBkWindow *pWnd = pT->FindChildByCmdID(uItemID);

		if (!pWnd)
			return FALSE;

		if (!pWnd->IsClass(CBkRichText2Wnd::GetClassName()) &&
			!pWnd->IsClass(CBkRichText::GetClassName()) &&
			!pWnd->IsClass(CBkRichTextEx::GetClassName()))
			return FALSE;

		TiXmlDocument xmlDoc;

		{ // Free stack
			CStringA strXml;
			strXml = L'<';
			strXml += CBkRichText::GetClassName();
			strXml += L'>';
			strXml += lpszXml;
			strXml += L'<';
			strXml += L'/';
			strXml += CBkRichText::GetClassName();
			strXml += L'>';
			xmlDoc.Parse(strXml, NULL, TIXML_ENCODING_UTF8);
		}

		if (xmlDoc.Error())
			return FALSE;

		if (pWnd->IsClass(CBkRichText2Wnd::GetClassName()))
		{
			CBkRichText2Wnd *pWndRichText = (CBkRichText2Wnd *)pWnd;

			TiXmlElement* pRootElm = xmlDoc.RootElement();
			if (pRootElm != NULL)
			{
				pWndRichText->LoadChilds(pRootElm->FirstChildElement());
			}
		}
		else
		{
			CBkRichText *pWndRichText = (CBkRichText *)pWnd;
			pWndRichText->LoadChilds(xmlDoc.RootElement());
		}

		pT->_InvalidateControl(pWnd);

		if (pWnd->IsVisible(TRUE))
			pWnd->BkSendMessage(WM_SHOWWINDOW, TRUE);

		return TRUE;
	}

	BOOL SetItemText(UINT uItemID, LPCTSTR lpszText)
	{
		T* pT = static_cast<T*>(this);
		CBkWindow *pWnd = pT->FindChildByCmdID(uItemID);

		if (pWnd)
		{
			pT->_InvalidateControl(pWnd);

			pWnd->SetInnerText(lpszText);

			pT->_InvalidateControl(pWnd);

			return TRUE;
		}

		return FALSE;
	}

	CString GetItemText(UINT uItemID)
	{
		T* pT = static_cast<T*>(this);
		CBkWindow *pWnd = pT->FindChildByCmdID(uItemID);
		CString strItemText = _T("");

		if (pWnd)
		{
			pT->_InvalidateControl(pWnd);

			strItemText = pWnd->GetInnerText();

			pT->_InvalidateControl(pWnd);

			return strItemText;
		}

		return strItemText;
	}

	BOOL SetItemAttribute(UINT uItemID, LPCSTR lpszAttrib, LPCSTR lpszValue)
	{
		T* pT = static_cast<T*>(this);
		CBkWindow *pWnd = pT->FindChildByCmdID(uItemID);

		if (pWnd)
		{
			pT->_InvalidateControl(pWnd);

			pWnd->SetAttribute(lpszAttrib, lpszValue, FALSE);

			pT->_InvalidateControl(pWnd);

			return TRUE;
		}

		return FALSE;
	}

	BOOL SetItemStringAttribute(UINT uItemID, LPCSTR lpszAttrib, LPCTSTR lpszValue)
	{
		return SetItemAttribute(uItemID, lpszAttrib, CT2A(lpszValue, CP_UTF8));
	}

	BOOL SetItemIntAttribute(UINT uItemID, LPCSTR lpszAttrib, int nValue)
	{
		CStringA strValue;
		strValue.Format("%d", nValue);

		return SetItemAttribute(uItemID, lpszAttrib, strValue);
	}

	BOOL SetItemDWordAttribute(UINT uItemID, LPCSTR lpszAttrib, DWORD dwValue)
	{
		CStringA strValue;
		strValue.Format("%u", dwValue);

		return SetItemAttribute(uItemID, lpszAttrib, strValue);
	}

	BOOL SetItemColorAttribute(UINT uItemID, LPCSTR lpszAttrib, COLORREF crValue)
	{
		CStringA strValue;
		strValue.Format("%02X%02X%02X", GetRValue(crValue), GetGValue(crValue), GetBValue(crValue));

		return SetItemAttribute(uItemID, lpszAttrib, strValue);
	}

	BOOL GetItemCheck(UINT uItemID)
	{
		T* pT = static_cast<T*>(this);
		CBkWindow *pWnd = pT->FindChildByCmdID(uItemID);

		if (pWnd)
			return pWnd->IsChecked();

		return FALSE;
	}

	BOOL SetItemCheck(UINT uItemID, BOOL bCheck)
	{
		T* pT = static_cast<T*>(this);
		CBkWindow *pWnd = pT->FindChildByCmdID(uItemID);

		if (pWnd)
		{
			if (bCheck)
				pWnd->ModifyState(BkWndState_Check, 0);
			else
				pWnd->ModifyState(0, BkWndState_Check);

			pT->_InvalidateControl(pWnd);

			return TRUE;
		}

		return FALSE;
	}

	BOOL EnableItem(UINT uItemID, BOOL bEnable)
	{
		T* pT = static_cast<T*>(this);
		CBkWindow *pWnd = pT->FindChildByCmdID(uItemID);

		if (pWnd)
		{
			if (bEnable)
				pWnd->ModifyState(0, BkWndState_Disable);
			else
				pWnd->ModifyState(BkWndState_Disable, BkWndState_Hover);

			pT->_InvalidateControl(pWnd);

			return TRUE;
		}

		return FALSE;
	}

	BOOL IsItemEnable(UINT uItemID, BOOL bCheckParent = FALSE)
	{
		T* pT = static_cast<T*>(this);
		CBkWindow *pWnd = pT->FindChildByCmdID(uItemID);

		if (pWnd)
			return !pWnd->IsDisabled(bCheckParent);

		return FALSE;
	}

	BOOL SetItemVisible(UINT uItemID, BOOL bVisible)
	{
		T* pT = static_cast<T*>(this);
		CBkWindow *pWnd = pT->FindChildByCmdID(uItemID);

		if (pWnd)
		{
			pWnd->BkSendMessage(WM_SHOWWINDOW, (WPARAM)bVisible);

			pT->_InvalidateControl(pWnd, FALSE);

			return TRUE;
		}

		return FALSE;
	}

	BOOL IsItemVisible(UINT uItemID, BOOL bCheckParent = FALSE)
	{
		T* pT = static_cast<T*>(this);
		CBkWindow *pWnd = pT->FindChildByCmdID(uItemID);

		if (pWnd)
			return pWnd->IsVisible(bCheckParent);

		return FALSE;
	}

	BOOL GetItemRect(UINT uItemID, RECT &rcItem)
	{
		T* pT = static_cast<T*>(this);
		CBkWindow *pWnd = pT->FindChildByCmdID(uItemID);
		if (!pWnd)
			return FALSE;

		pWnd->GetRect(&rcItem);

		return TRUE;
	}

	BOOL SetItemNeedReDraw(UINT uItemID)
	{
		T* pT = static_cast<T*>(this);
		CBkWindow *pWnd = pT->FindChildByCmdID(uItemID);
		if (!pWnd)
			return FALSE;

		pWnd->NotifyInvalidate();

		return TRUE;
	}

	BOOL SetItemRect(UINT uItemID, RECT &rcItem)
	{
		T* pT = static_cast<T*>(this);
		CBkWindow *pWnd = pT->FindChildByCmdID(uItemID);
		if (!pWnd)
			return FALSE;

		pWnd->SetRect(&rcItem);

		return TRUE;
	}

	BOOL SetTabCurSel(UINT uItemID, int nPage)
	{
		T* pT = static_cast<T*>(this);
		CBkWindow *pWnd = pT->FindChildByCmdID(uItemID);
		if (!pWnd)
			return FALSE;

		if (!pWnd->IsClass(CBkTabCtrl::GetClassName()))
			return FALSE;

		BOOL bRet = ((CBkTabCtrl *)pWnd)->SetCurSel(nPage);

		return bRet;
	}

	BOOL SetTabTitle(UINT uItemID, int nPage, LPCTSTR lpszTitle)
	{
		T* pT = static_cast<T*>(this);
		CBkWindow *pWnd = pT->FindChildByCmdID(uItemID);
		if (!pWnd)
			return FALSE;

		if (!pWnd->IsClass(CBkTabCtrl::GetClassName()))
			return FALSE;

		BOOL bRet = ((CBkTabCtrl *)pWnd)->SetTabTitle(nPage, lpszTitle);

		return bRet;
	}

	int GetTabCurSel(UINT uItemID)
	{
		T* pT = static_cast<T*>(this);
		CBkWindow *pWnd = pT->FindChildByCmdID(uItemID);
		if (!pWnd)
			return -1;

		if (!pWnd->IsClass(CBkTabCtrl::GetClassName()))
			return -1;

		return ((CBkTabCtrl *)pWnd)->GetCurSel();
	}

	CBkWindow* GetBkItem(UINT uItemID)
	{
		T* pT = static_cast<T*>(this);
		CBkWindow *pWnd = pT->FindChildByCmdID(uItemID);

		return pWnd;
	}

	BOOL IsTabPageVisible(UINT uItemID, int nPage)
	{
		T* pT = static_cast<T*>(this);
		CBkWindow *pWnd = pT->FindChildByCmdID(uItemID);
		if (!pWnd)
			return FALSE;

		if (!pWnd->IsClass(CBkTabCtrl::GetClassName()))
			return FALSE;

		return ((CBkTabCtrl *)pWnd)->IsPageVisible(nPage);
	}

	BOOL SetTabPageVisible(UINT uItemID, int nPage, BOOL bVisible)
	{
		T* pT = static_cast<T*>(this);
		CBkWindow *pWnd = pT->FindChildByCmdID(uItemID);
		if (!pWnd)
			return FALSE;

		if (!pWnd->IsClass(CBkTabCtrl::GetClassName()))
			return FALSE;


		((CBkTabCtrl *)pWnd)->SetPageVisible(nPage, bVisible);

		return TRUE;
	}

	BOOL SetItemIconHandle(UINT uItemID, HICON hIcon)
	{
		T* pT = static_cast<T*>(this);
		CBkWindow *pWnd = pT->FindChildByCmdID(uItemID);
		if (!pWnd)
			return FALSE;

		if (!pWnd->IsClass(CBkIconWnd::GetClassName()))
			return FALSE;

		((CBkIconWnd *)pWnd)->AttachIcon(hIcon);

		pT->_InvalidateControl(pWnd);

		return TRUE;
	}

	BOOL SetItemPaintHook(UINT uItemID, IBkWindowPaintHook* pPaintHook)
	{
		T* pT = static_cast<T*>(this);
		CBkWindow *pWnd = pT->FindChildByCmdID(uItemID);
		if (!pWnd)
			return FALSE;

		pWnd->SetPaintHook(pPaintHook);

		return TRUE;
	}

	BOOL SetItemMouseHook(UINT uItemID, IBkWindowMouseHook *pMouseHook)
	{
		T* pT = static_cast<T*>(this);
		CBkWindow *pWnd = pT->FindChildByCmdID(uItemID);
		if (!pWnd)
			return FALSE;

		pWnd->SetMouseHook(pMouseHook);

		return TRUE;
	}


	BOOL FormatRichText(UINT uItemID, UINT uStringID, ...)
	{
		CString strFormat = BkString::Get(uStringID);
		va_list args;
		CString strText;

		va_start(args, uStringID);

		strText.FormatV(strFormat, args);

		return SetRichText(uItemID, strText);
	}

	BOOL FormatRichText(UINT uItemID, LPCTSTR lpszFormat, ...)
	{
		va_list args;
		CString strText;

		va_start(args, lpszFormat);

		strText.FormatV(lpszFormat, args);

		return SetRichText(uItemID, strText);
	}

	BOOL FormatItemText(UINT uItemID, LPCTSTR lpszFormat, ...)
	{
		va_list args;
		CString strText;

		va_start(args, lpszFormat);

		strText.FormatV(lpszFormat, args);

		return SetItemText(uItemID, strText);
	}
};
#endif