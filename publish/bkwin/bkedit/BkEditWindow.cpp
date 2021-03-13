#include "stdafx.h"
#include "BkEditWindow.h"
#include "BkTextServices.h"

#define DEFAULT_COLOR  CARGB(255, 255, 255, 255)

CBkEditWindow::CBkEditWindow()
{
	m_pTextServiceHost = new CBkTextServiceHost(*this);	
	m_pTextServiceHost->SetDefaultFont(BkFontPool::GetFont(0, IsDpiAwareEnable()));
	m_pTextServiceHost->CreateTextServices();
	m_bFocusable = TRUE;
}

CBkEditWindow::~CBkEditWindow()
{
	if(m_pTextServiceHost != NULL) 
	{
		m_pTextServiceHost->SetIBkWndTimer(NULL);
		m_pTextServiceHost->Release();
		m_pTextServiceHost = NULL;
	}
}

BOOL CBkEditWindow::Load(TiXmlElement* pTiXmlElem)
{
	BOOL bSuccess = CBkWindow::Load(pTiXmlElem);
	UpdateTextServiceFont();
	UpdateTextServiceTextColor();
	m_pTextServiceHost->SetText(L"²âÊÔÎÄ°æ");

	return bSuccess;
}

void CBkEditWindow::SetIBkWndTimer(BkTimer::IBkWndTimer* pTimer)
{
	if(m_pTextServiceHost != NULL) 
	{
		m_pTextServiceHost->SetIBkWndTimer(pTimer);
	}
}

CString CBkEditWindow::GetEditText()
{
	if(m_pTextServiceHost != NULL) 
	{
		return m_pTextServiceHost->GetText();
	}

	return L"";
}

void CBkEditWindow::SetEditText(LPCWSTR lpszText)
{
	if (m_pTextServiceHost != NULL) 
	{
		m_pTextServiceHost->SetText(lpszText);
	}
}

HWND CBkEditWindow::GetHWND()
{
	return GetRealContainerWnd();
}

CRect CBkEditWindow::GetViewInset()
{
	return CRect(0, 0, 0, 0);
}

CRect CBkEditWindow::GetWindowRect()
{
	CRect rcWindow;
	CBkWindow::GetRect(rcWindow);
	rcWindow.MoveToXY(0, 0);
	return rcWindow;
}

void CBkEditWindow::Invalidate()
{
	NotifyInvalidate();
}


void CBkEditWindow::OnBkTimer(UINT uItmerID)
{
}

void CBkEditWindow::OnDestroy()
{
	SetMsgHandled(FALSE);
	if(m_pTextServiceHost != NULL) 
	{
		m_pTextServiceHost->SetIBkWndTimer(NULL);
		m_pTextServiceHost->Release();
		m_pTextServiceHost = NULL;
	}
}

void CBkEditWindow::OnPaint(CDCHandle dc)
{
	SetMsgHandled(FALSE);
	if(!ValidateTextService()) 
	{
		return;
	}

	if(!PrepareBmp())
	{
		return;
	}

	if(!IsFocused() && m_pTextServiceHost->GetText().IsEmpty()) 
	{
		return;
	}

	SetMsgHandled(TRUE);
	CDCHandle dcDesktop(::GetDC(NULL));
	CDC dcMem;

	dcMem.CreateCompatibleDC(dcDesktop);
	::ReleaseDC(NULL, dcDesktop);

	HBITMAP hBimpOld = dcMem.SelectBitmap(m_bkbmp);
	dcMem.SetBkMode(TRANSPARENT);
	//::SetGraphicsMode(dcMem, GM_COMPATIBLE);	

	LONG lPos =0;
	HRESULT hr= m_pTextServiceHost->GetTextService()->TxGetVScroll(NULL,NULL,&lPos,NULL,NULL);
	RECTL rcL= {0, 0, m_bkbmp.GetWidth(), m_bkbmp.GetHeight()};
	CRect rcClient(0, 0, m_bkbmp.GetWidth(), m_bkbmp.GetHeight());

	COLORREF crText = m_pTextServiceHost->GetTextColor();
	m_bkbmp.SetAllPixel(CARGB(255 - GetRValue(crText), 255 - GetGValue(crText), 255 - GetBValue(crText)));
	dcMem.StretchBlt(0, 0, m_bkbmp.GetWidth(), m_bkbmp.GetHeight(), dc, m_rcWindow.left, m_rcWindow.top, m_rcWindow.Width(), m_rcWindow.Height(), SRCCOPY);
	m_bkbmp.SetAlpha(255);

	m_pTextServiceHost->GetTextService()->TxDraw(
		DVASPECT_CONTENT,          // Draw Aspect
		/*-1*/0,                        // Lindex
		NULL,                    // Info for drawing optimazation
		NULL,                    // target device information
		dcMem,            // Draw device HDC
		NULL,                        // Target device HDC
		&rcL,            // Bounding client rectangle
		NULL,             // Clipping rectangle for metafiles
		&rcClient,        // Update rectangle
		NULL,                        // Call back function
		NULL,                    // Call back parameter
		TXTVIEW_ACTIVE);
	dcMem.SelectBitmap(hBimpOld);


	PKARGB pDwPixel = (PKARGB)m_bkbmp.m_pBytes;
	const int nPixelCount = m_bkbmp.GetWidth() * m_bkbmp.GetHeight();
	for (int i = 0; i < nPixelCount; i++, pDwPixel++)
	{
		PBYTE pAlpha = ((PBYTE)pDwPixel) + 3;
		if (*pAlpha == 0)
		{
			*pAlpha = 255;
		}
		else 
		{
			*pDwPixel = 0;
		}
	}

	CBkBitmap::AlphaBlend(dc, m_rcWindow.left, m_rcWindow.top, m_rcWindow.Width(), m_rcWindow.Height(), m_bkbmp, 0, 0, 
		m_bkbmp.GetWidth(), m_bkbmp.GetHeight(), m_nAlphaTxt);
}

LRESULT CBkEditWindow::OnMsgDispathFocusWnd(UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	SetMsgHandled(FALSE);
	TxSendMessage(uMsg, wParam, lParam);
	return S_OK;
}

void CBkEditWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
	CPoint ptNew = ConvertToLocalEditPos(point);
	TxSendMessage(WM_LBUTTONDOWN, (WPARAM)nFlags, MAKELPARAM(ptNew.x, ptNew.y));	
}

void CBkEditWindow::OnLButtonUp(UINT nFlags, CPoint point)
{
	CPoint ptNew = ConvertToLocalEditPos(point);
	TxSendMessage(WM_LBUTTONUP, (WPARAM)nFlags, MAKELPARAM(ptNew.x, ptNew.y));	
}

void CBkEditWindow::OnSetFocus(CWindow wndOld)
{
	SetFocused(true);
	if(ValidateTextService()) 
	{
		m_pTextServiceHost->SetFocused(true);
	}
}

void CBkEditWindow::OnKillFocus(CWindow wndFocus)
{
	SetFocused(false);
	if(ValidateTextService()) 
	{
		m_pTextServiceHost->SetFocused(false);
	}
}

void CBkEditWindow::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint ptNew = ConvertToLocalEditPos(point);
	TxSendMessage(WM_MOUSEMOVE, (WPARAM)nFlags, MAKELPARAM(ptNew.x, ptNew.y));	
}

void CBkEditWindow::OnMouseLeave()
{
	TxSendMessage(WM_MOUSEMOVE, 0, 0);	
}

bool CBkEditWindow::PrepareBmp()
{	
	if(m_rcWindow.IsRectEmpty()) 
	{
		return false;
	}

	if (m_bkbmp.m_hBitmap != NULL && 
		m_bkbmp.GetWidth() == m_rcWindow.Width() && 
		m_bkbmp.GetHeight() == m_rcWindow.Height()) 
	{
		return true;
	}

	m_bkbmp.DeleteObject();
	m_bkbmp.CreateDIBSection(m_rcWindow.Width(), m_rcWindow.Height());

	return m_bkbmp.m_hBitmap != NULL;
}

bool CBkEditWindow::ValidateTextService() const
{
	return m_pTextServiceHost && m_pTextServiceHost->Validate();
}

void CBkEditWindow::TxSendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(ValidateTextService())
	{
		LRESULT lResult = 0;
		m_pTextServiceHost->GetTextService()->TxSendMessage(uMsg, wParam, lParam, &lResult);
	}
}

CPoint CBkEditWindow::ConvertToLocalEditPos(const CPoint& pt)
{
	return CPoint(pt.x - m_rcWindow.left, pt.y - m_rcWindow.top);
}

HRESULT CBkEditWindow::SetAttribute(CStringA strAttribName, CStringA strValue, BOOL bLoading)
{
	if (strAttribName == "class" || strAttribName == "crtext" || strAttribName == "font") 
	{
		HRESULT hResult = __super::SetAttribute(strAttribName, strValue, bLoading);
		if(strAttribName == "crtext" || strAttribName == "class") 
		{
			UpdateTextServiceTextColor();
		}
		else if(strAttribName == "font") 
		{
			UpdateTextServiceFont();
		}

		return hResult;
	}

	return __super::SetAttribute(strAttribName, strValue, bLoading);;
}

void CBkEditWindow::UpdateTextServiceFont()
{
	if(!ValidateTextService()) 
	{
		return;
	}

	DWORD wFtText = 0;
	if (m_wFtText != 0)
	{
		wFtText = m_wFtText;
	}
	else if (GetStyle().m_wFtText != 0)
	{
		wFtText = GetStyle().m_wFtText;
	}

	m_pTextServiceHost->SetDefaultFont(BkFontPool::GetFont(wFtText, IsDpiAwareEnable()));
}

void CBkEditWindow::UpdateTextServiceTextColor()
{
	if(!ValidateTextService()) 
	{
		return;
	}

	COLORREF crDraw = CLR_INVALID;
	if (m_crText != CLR_INVALID)
	{
		crDraw = m_crText;
	}
	else if (GetStyle().m_crText != CLR_INVALID)
	{
		crDraw = GetStyle().m_crText;
	}
	m_pTextServiceHost->SetColorText(crDraw);
}

void CBkEditWindow::OnWindowPosChanged(LPWINDOWPOS lpWndPos)
{
	CRect rcOld = m_rcWindow;
	CBkWindow::OnWindowPosChanged(lpWndPos);
	if(ValidateTextService() && (rcOld.Width() != m_rcWindow.Width() || rcOld.Height() != m_rcWindow.Height())) 
	{
		CRect rcClient(m_rcWindow);
		rcClient.MoveToXY(0, 0);
		m_pTextServiceHost->SetClientRect(rcClient);
	}
}
