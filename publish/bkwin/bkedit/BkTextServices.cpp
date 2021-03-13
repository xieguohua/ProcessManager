#include "stdafx.h"
#include "BkTextServices.h"

#ifndef LY_PER_INCH
#define LY_PER_INCH 1440
#endif

#ifndef HIMETRIC_PER_INCH
#define HIMETRIC_PER_INCH 2540
#endif

// Convert Device Pixels to Himetric
LONG DtoHimetric(LONG d, LONG dPerInch)
{
	return (LONG) MulDiv(d, HIMETRIC_PER_INCH, dPerInch);
}

// Convert Himetric Device pixels
LONG HimetrictoD(LONG lHimetric, LONG dPerInch)
{
	return (LONG) MulDiv(lHimetric, dPerInch, HIMETRIC_PER_INCH);
}

EXTERN_C const IID IID_ITextServices =   // 8d33f740-cf58-11ce-a89d-00aa006cadc5
{
	0x8d33f740,
	0xcf58,
	0x11ce,
	{0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

EXTERN_C const IID IID_ITextHost =   /* c5bdd8d0-d26e-11ce-a89e-00aa006cadc5 */
{
	0xc5bdd8d0,
	0xd26e,
	0x11ce,
	{0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

HRESULT CBkTextServiceHelper::CreateTextServices(IUnknown *punkOuter, ITextHost *pITextHost, IUnknown **ppUnk)
{
	if(!m_funCreateTextServices) 
	{
		return E_NOTIMPL;
	}

	return m_funCreateTextServices(punkOuter, pITextHost, ppUnk);
}

CBkTextServiceHelper::CBkTextServiceHelper()
{
	m_rich20=LoadLibrary(_T("riched20.dll"));
	if(m_rich20) m_funCreateTextServices= (PCreateTextServices)GetProcAddress(m_rich20,"CreateTextServices");
}

CBkTextServiceHelper::~CBkTextServiceHelper()
{

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CBkTextServiceHost::CBkTextServiceHost(IBkRichTextWindow& textWnd) 
	: m_bkWindow(textWnd)
	, m_pITextServices(NULL)
	, m_bShowCaret(FALSE)
	, m_fUiActive(FALSE)
	, cRefs(1)
	, m_nMaxCharLength(1000000)
	, m_dwStyle(0)
	, m_lAccelPos(-1)
	, m_pIBkWndTimer(NULL)
	, m_crText(RGB(0, 0, 0))
{
	m_rcInset.SetRectEmpty();
	m_rcClient.SetRectEmpty();
	memset(&m_charFormat, 0, sizeof(m_charFormat));
	m_charFormat.cbSize = sizeof(m_charFormat);

	UpdateDefaultParaFormat();
	m_sizelExtent.cx = m_sizelExtent.cy = 0;
	
}

CBkTextServiceHost::~CBkTextServiceHost(void)
{
	if(m_pITextServices != NULL) 
	{
		m_pITextServices->Release();
		m_pITextServices = NULL;
	}
}

void CBkTextServiceHost::SetDefaultFont(HFONT hFont)
{
	CFontHandle font(hFont);

	LOGFONT logFont = {0};
	if (!font.GetLogFont(logFont)) 
	{
		return;
	}

	SetDefaultFont(logFont);
}

void CBkTextServiceHost::SetDefaultFont(const LOGFONT& logFont)
{

	HDC hdc=GetDC(NULL);
	LONG yPixPerInch = GetDeviceCaps(hdc, LOGPIXELSY);
	ReleaseDC(NULL,hdc);

	memset(&m_charFormat, 0, sizeof(m_charFormat));
	m_charFormat.cbSize = sizeof(CHARFORMAT2W);
	m_charFormat.dwMask = CFM_SIZE | CFM_OFFSET | CFM_FACE | CFM_CHARSET | CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;
	m_charFormat.yHeight = abs(MulDiv(logFont.lfHeight, LY_PER_INCH, yPixPerInch));
	m_charFormat.yOffset = 0;
	m_charFormat.wWeight = logFont.lfWeight;
	m_charFormat.dwEffects = 0;
	m_charFormat.crTextColor = m_crText;
	if (logFont.lfItalic) 
	{
		m_charFormat.dwEffects |= CFE_ITALIC;
	}

	if (logFont.lfUnderline) 
	{
		m_charFormat.dwEffects |= CFE_UNDERLINE;
	}

	if (logFont.lfWeight == FW_BOLD) 
	{
		m_charFormat.dwEffects |= CFE_BOLD;
	}

	m_charFormat.bCharSet = logFont.lfCharSet;
	m_charFormat.bPitchAndFamily = logFont.lfPitchAndFamily;
	wcscpy(m_charFormat.szFaceName, logFont.lfFaceName);

	if(Validate()) 
	{
		m_pITextServices->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 0);
	}
}


void CBkTextServiceHost::SetColorText(COLORREF crText)
{
	if (crText == m_crText || crText == CLR_INVALID) {
		return;
	}
	m_crText = crText;
	m_charFormat.crTextColor = m_crText;
	if(Validate()) 
	{
		m_pITextServices->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 0);
	}
}

void CBkTextServiceHost::UpdateDefaultParaFormat()
{
	memset(&m_paraFormat, 0, sizeof(m_paraFormat));
	m_paraFormat.cbSize = sizeof(m_paraFormat);
	m_paraFormat.dwMask = PFM_ALL;
	m_paraFormat.cTabCount = 1;
	m_paraFormat.rgxTabs[0] = lDefaultTab;

	if(m_dwStyle & ES_CENTER)
		m_paraFormat.wAlignment = PFA_CENTER;
	else if(m_dwStyle & ES_RIGHT)
		m_paraFormat.wAlignment = PFA_RIGHT;
	else
		m_paraFormat.wAlignment = PFA_LEFT;

	if(Validate()) 
	{
		m_pITextServices->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
	}
}


void CBkTextServiceHost::CreateTextServices()
{
	if (m_pITextServices != NULL) 
	{
		return;
	}

	CComPtr<IUnknown> pUnk;
	// Create Text Services component
	if(SUCCEEDED(CBkTextServiceHelper().CreateTextServices(NULL, this, &pUnk))) 
	{
		pUnk->QueryInterface(IID_ITextServices, (void **)&m_pITextServices);
	}
}


ITextServices* CBkTextServiceHost::GetTextService()
{
	return m_pITextServices;
}

POINT CBkTextServiceHost::GetCaretPos()
{
	return m_ptCaret;
}

void CBkTextServiceHost::SetIBkWndTimer(BkTimer::IBkWndTimer* pTimer)
{
	BkTimer::IBkWndTimer* pTimerOld = m_pIBkWndTimer;
	if (pTimerOld) 
	{
		pTimerOld->KillAlllTimer(this);
	}

	m_pIBkWndTimer = pTimer;
}

void CBkTextServiceHost::SetClientRect(const CRect& rcClient)
{
	if(m_rcClient == rcClient) 
	{
		return;
	}
	
	m_rcClient = rcClient;

	HWND hWnd = m_bkWindow.GetHWND();
	HDC hdc= GetDC(hWnd);
	LONG xPerInch = ::GetDeviceCaps(hdc, LOGPIXELSX);
	LONG yPerInch =    ::GetDeviceCaps(hdc, LOGPIXELSY);
	ReleaseDC(hWnd, hdc);

	m_sizelExtent.cx = DtoHimetric(m_rcClient.Width(), xPerInch);
	m_sizelExtent.cy = DtoHimetric(m_rcClient.Height(), yPerInch);

	m_rcInset.SetRectEmpty();
// 	m_rcInset.left= DtoHimetric(rcInsetPixel.left, xPerInch);
// 	m_rcInset.right= DtoHimetric(rcInsetPixel.right, xPerInch);
// 	m_rcInset.top = DtoHimetric(rcInsetPixel.top, yPerInch);
// 	m_rcInset.bottom = DtoHimetric(rcInsetPixel.bottom, yPerInch);

	if(Validate()) 
	{
		m_pITextServices->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, 0);
	}
}

HRESULT _stdcall CBkTextServiceHost::QueryInterface( REFIID riid, void **ppvObject )
{
	HRESULT hr = E_NOINTERFACE;
	*ppvObject = NULL;

	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_ITextHost))
	{
		AddRef();
		*ppvObject = (ITextHost *) this;
		hr = S_OK;
	}

	return hr;
}

ULONG _stdcall CBkTextServiceHost::AddRef( void )
{
	return ++cRefs;
}

ULONG _stdcall CBkTextServiceHost::Release( void )
{
	ULONG c_Refs = --cRefs;

	if (c_Refs == 0)
	{
		delete this;
	}

	return c_Refs;
}


void CBkTextServiceHost::OnTxInPlaceDeactivate()
{
	if(m_pITextServices != NULL)
	{
		m_pITextServices->OnTxInPlaceDeactivate();
	}
}

void CBkTextServiceHost::SetText(LPCWSTR lpszText)
{
	if (m_pITextServices != NULL) 
	{
		m_pITextServices->TxSetText(lpszText);
	}
}

CString CBkTextServiceHost::GetText()
{
	if(m_pITextServices == NULL) 
	{
		return L"";
	}

	BSTR bstrText = NULL;
	m_pITextServices->TxGetText(&bstrText);
	if(bstrText == NULL) 
	{
		return L"";
	}

	CString strText = bstrText;
	::SysFreeString(bstrText);

	return strText;
}

COLORREF CBkTextServiceHost::GetTextColor() const
{
	return m_crText;
}

bool CBkTextServiceHost::Validate() const
{
	return m_pITextServices != NULL;
}

void CBkTextServiceHost::SetFocused(bool bFocus)
{
	if (!Validate())
	{
		return;
	}

	m_fUiActive = bFocus;
	if(bFocus) 
	{
		m_pITextServices->OnTxUIActivate();
		m_pITextServices->TxSendMessage(WM_SETFOCUS, 0, 0, 0);
	}
	else
	{
		m_pITextServices->OnTxUIDeactivate();
		m_pITextServices->TxSendMessage(WM_KILLFOCUS, 0, 0, 0);
	}
}

void CBkTextServiceHost::OnBkTimer(UINT uItmerID)
{
	if (m_pITextServices != NULL) 
	{
		m_pITextServices->TxSendMessage(WM_TIMER, uItmerID, 0, NULL);
	}
}

//////////////////////////////////////////////////////////////////////////
// ITextHost
HRESULT CBkTextServiceHost::TxGetViewInset( LPRECT prc )
{
	*prc= m_rcInset;
	return S_OK;
}

HRESULT CBkTextServiceHost::TxGetCharFormat( const CHARFORMAT **ppCF )
{
	*ppCF = &m_charFormat;
	return S_OK;
}


HRESULT CBkTextServiceHost::TxGetParaFormat( const PARAFORMAT **ppPF )
{
	*ppPF = &m_paraFormat;
	return S_OK;
}

HRESULT CBkTextServiceHost::TxGetClientRect( LPRECT prc )
{
	if (NULL != prc) 
	{
		*prc = m_rcClient;
	}

	return S_OK;
}

HRESULT CBkTextServiceHost::TxDeactivate( LONG lNewState )
{
	m_fUiActive = FALSE;
	return S_OK;
}

HRESULT CBkTextServiceHost::TxActivate( LONG * plOldState )
{
	*plOldState = m_fUiActive;
	m_fUiActive=TRUE;

	return S_OK;
}

BOOL CBkTextServiceHost::TxClientToScreen( LPPOINT lppt )
{
	return ::ClientToScreen(m_bkWindow.GetHWND(), lppt);
}

BOOL CBkTextServiceHost::TxScreenToClient( LPPOINT lppt )
{
	return ::ScreenToClient(m_bkWindow.GetHWND(), lppt);
}

void CBkTextServiceHost::TxSetCursor( HCURSOR hcur, BOOL fText )
{
	::SetCursor(hcur);
}

void CBkTextServiceHost::TxSetFocus()
{
	//m_pRichEdit->SetFocus();
}

void CBkTextServiceHost::TxSetCapture( BOOL fCapture )
{
	if(fCapture)
	{
		::SetCapture(m_bkWindow.GetHWND());
	}
	else 
	{
		::ReleaseCapture();
	}
}

void CBkTextServiceHost::TxScrollWindowEx( INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll )
{
	m_bkWindow.Invalidate();
}

void CBkTextServiceHost::TxKillTimer( UINT idTimer )
{
	if(m_pIBkWndTimer != NULL) 
	{
		m_pIBkWndTimer->KillTimer(this, idTimer);
	}
}

BOOL CBkTextServiceHost::TxSetTimer( UINT idTimer, UINT uTimeout )
{
	if(m_pIBkWndTimer != NULL) 
	{
		return m_pIBkWndTimer->SetTimer(this, idTimer, uTimeout);
	}

	return TRUE;
}

BOOL CBkTextServiceHost::TxSetCaretPos( INT x, INT y )
{
	m_ptCaret.x = x;
	m_ptCaret.y = y;

	return TRUE;
}

BOOL CBkTextServiceHost::TxShowCaret( BOOL fShow )
{
	if(fShow && !m_fUiActive) 
	{
		return FALSE;
	}

	m_bkWindow.Invalidate();
	return TRUE;
}

BOOL CBkTextServiceHost::TxCreateCaret( HBITMAP hbmp, INT xWidth, INT yHeight )
{
	::CreateCaret(m_bkWindow.GetHWND(), hbmp, xWidth, yHeight);
	::HideCaret(m_bkWindow.GetHWND());

	return TRUE;
}

HDC CBkTextServiceHost::TxGetDC()
{
	return ::GetDC(NULL);
}

INT CBkTextServiceHost::TxReleaseDC( HDC hdc )
{
	return ::ReleaseDC(NULL, hdc);
}

BOOL CBkTextServiceHost::TxShowScrollBar( INT fnBar, BOOL fShow )
{
	int wBar=0;
	switch(fnBar)
	{
	case SB_BOTH:
		//wBar=SSB_BOTH;
		break;
	case SB_VERT:
		//wBar=SSB_VERT;
		break;
	case SB_HORZ:
		//wBar=SSB_HORZ;
		break;
	}

	return TRUE;
}

BOOL CBkTextServiceHost::TxEnableScrollBar( INT fuSBFlags, INT fuArrowflags )
{
	int wBar=0;
	switch(fuSBFlags)
	{
	case SB_BOTH:
		//wBar=SSB_BOTH;
		break;
	case SB_VERT:
		//wBar=SSB_VERT;
		break;
	case SB_HORZ:
		//wBar=SSB_HORZ;
		break;
	}

	return TRUE;
}

BOOL CBkTextServiceHost::TxSetScrollRange( INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw )
{
	return TRUE;
	//return m_pRichEdit->SetScrollRange(fnBar!=SB_HORZ,nMinPos,nMaxPos,fRedraw);
}

BOOL CBkTextServiceHost::TxSetScrollPos( INT fnBar, INT nPos, BOOL fRedraw )
{
// 	BOOL bRet=FALSE;
// 	if(m_pRichEdit->m_fScrollPending) return TRUE;
// 	m_pRichEdit->m_fScrollPending=TRUE;
// 	bRet= m_pRichEdit->SetScrollPos(fnBar!=SB_HORZ,nPos,fRedraw);
// 	m_pRichEdit->m_fScrollPending=FALSE;
// 	return bRet;
	m_bkWindow.Invalidate();
	return TRUE;
}

void CBkTextServiceHost::TxInvalidateRect( LPCRECT prc, BOOL fMode )
{
	m_bkWindow.Invalidate();
}

void CBkTextServiceHost::TxViewChange( BOOL fUpdate )
{
	if(fUpdate)
	{
		m_bkWindow.Invalidate();
	}
}

COLORREF CBkTextServiceHost::TxGetSysColor( int nIndex )
{
	return ::GetSysColor(nIndex);
}

HRESULT CBkTextServiceHost::TxGetBackStyle( TXTBACKSTYLE *pstyle )
{
	*pstyle = TXTBACK_TRANSPARENT;
	return S_OK;
}

HRESULT CBkTextServiceHost::TxGetMaxLength( DWORD *plength )
{
	*plength = m_nMaxCharLength;
	return S_OK;
}

HRESULT CBkTextServiceHost::TxGetScrollBars( DWORD *pdwScrollBar )
{
	*pdwScrollBar =  m_dwStyle & (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL |
		ES_AUTOHSCROLL | ES_DISABLENOSCROLL);

	return S_OK;
}

HRESULT CBkTextServiceHost::TxGetPasswordChar( TCHAR *pch )
{
	*pch = L'*';
	return S_OK;
}

HRESULT CBkTextServiceHost::TxGetAcceleratorPos( LONG *pcp )
{
	*pcp = m_lAccelPos;
	return S_OK;
}

HRESULT CBkTextServiceHost::TxGetExtent( LPSIZEL lpExtent )
{
	*lpExtent = m_sizelExtent;
	return S_OK;
}

HRESULT CBkTextServiceHost::OnTxCharFormatChange( const CHARFORMATW * pcf )
{
	return S_OK;
}

HRESULT CBkTextServiceHost::OnTxParaFormatChange( const PARAFORMAT * ppf )
{
	return S_OK;
}

HRESULT CBkTextServiceHost::TxGetPropertyBits( DWORD dwMask, DWORD *pdwBits )
{
	DWORD dwProperties = 0;

// 	if (m_pRichEdit->m_fRich)
// 	{
// 		dwProperties = TXTBIT_RICHTEXT;
// 	}

	if (m_dwStyle & ES_MULTILINE)
	{
		dwProperties |= TXTBIT_MULTILINE;
	}

	if (m_dwStyle & ES_READONLY)
	{
		dwProperties |= TXTBIT_READONLY;
	}

	if (m_dwStyle & ES_PASSWORD)
	{
		dwProperties |= TXTBIT_USEPASSWORD;
	}

	if (!(m_dwStyle & ES_NOHIDESEL))
	{
		dwProperties |= TXTBIT_HIDESELECTION;
	}

	if (/*m_fEnableAutoWordSel*/ true)
	{
		dwProperties |= TXTBIT_AUTOWORDSEL;
	}

// 	if (m_pRichEdit->m_fVertical)
// 	{
// 		dwProperties |= TXTBIT_VERTICAL;
// 	}

	if (/*m_pRichEdit->m_fWordWrap*/true)
	{
		dwProperties |= TXTBIT_WORDWRAP;
	}

// 	if (m_pRichEdit->m_fAllowBeep)
// 	{
// 		dwProperties |= TXTBIT_ALLOWBEEP;
// 	}

// 	if (m_pRichEdit->m_fSaveSelection)
// 	{
// 		dwProperties |= TXTBIT_SAVESELECTION;
// 	}

	*pdwBits = dwProperties & dwMask;

	return NOERROR;
}

HRESULT CBkTextServiceHost::TxNotify( DWORD iNotify, void *pv )
{
	if(iNotify==EN_REQUESTRESIZE)
	{
		return S_OK;
	}

	return S_OK;
}

HIMC CBkTextServiceHost::TxImmGetContext()
{
	return ImmGetContext(m_bkWindow.GetHWND());
}

HIMC CBkTextServiceHost::TxImmAssociateContext(HIMC himc)
{
	return ImmAssociateContext(m_bkWindow.GetHWND(), himc);
}

void CBkTextServiceHost::TxImmReleaseContext( HIMC himc )
{
	ImmReleaseContext(m_bkWindow.GetHWND(),himc);
}

HRESULT CBkTextServiceHost::TxGetSelectionBarWidth( LONG *plSelBarWidth )
{
	*plSelBarWidth = 0;
	return S_OK;
}
