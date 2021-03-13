//********************************************************************
//author:  hexujun@cmcm.com
//create time: 2020/6/16?20:07
//description: bkwin ITextServices
//*********************************************************************

#ifndef _H_BKTEXTSERVICES_H_
#define _H_BKTEXTSERVICES_H_

#include <Richedit.h>
#include <TextServ.h>
#include "IBkTimer.h"

class CBkTextServiceHelper
{
public:
	CBkTextServiceHelper();
	~CBkTextServiceHelper();
	HRESULT CreateTextServices( IUnknown *punkOuter, ITextHost *pITextHost, IUnknown **ppUnk );

protected:


	HINSTANCE    m_rich20; /**< richedit module */
	PCreateTextServices m_funCreateTextServices;  /**< »Øµ÷º¯Êý */
};

class IBkRichTextWindow
{
public:
	virtual HWND GetHWND() = 0;
	virtual CRect  GetViewInset() = 0;
	virtual CRect  GetWindowRect() = 0;
	virtual void    Invalidate() = 0;
};

class CBkTextServiceHost 
	: public ITextHost
	, public BkTimer::IBkTimerNotify
{
public:
  
    CBkTextServiceHost(IBkRichTextWindow& textWnd);
    ~CBkTextServiceHost(void);

	void SetDefaultFont(HFONT hFont);
	void SetDefaultFont(const LOGFONT& logFont);
	void SetColorText(COLORREF crText);

	void UpdateDefaultParaFormat();
	void CreateTextServices();

    ITextServices* GetTextService();
    POINT GetCaretPos();

	void SetIBkWndTimer(BkTimer::IBkWndTimer* pTimer);
	void SetClientRect(const CRect& rcClient);
    
	virtual ULONG _stdcall AddRef(void);
	virtual ULONG _stdcall Release(void);


	void OnTxInPlaceDeactivate();
	void SetText(LPCWSTR lpszText);
	CString GetText();
	COLORREF GetTextColor() const;

	bool Validate() const;
	void SetFocused(bool bFocus);

protected:
	virtual void OnBkTimer(UINT uItmerID);

	virtual HIMC  TxImmAssociateContext(HIMC himc);
    virtual HRESULT _stdcall QueryInterface(REFIID riid, void ** ppvObject);
    virtual HDC   TxGetDC();
    virtual INT   TxReleaseDC(HDC hdc);
    virtual BOOL  TxShowScrollBar(INT fnBar, BOOL fShow);
	BOOL  TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags);
    virtual BOOL  TxSetScrollRange(
        INT fnBar,
        LONG nMinPos,
        INT nMaxPos,
        BOOL fRedraw);
    virtual BOOL  TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw);
    virtual void TxInvalidateRect(LPCRECT prc, BOOL fMode);
    virtual void TxViewChange(BOOL fUpdate);
    virtual BOOL  TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);
    virtual BOOL  TxShowCaret(BOOL fShow);
    virtual BOOL  TxSetCaretPos(INT x, INT y);
    virtual BOOL  TxSetTimer(UINT idTimer, UINT uTimeout);
    virtual void TxKillTimer(UINT idTimer);
    virtual void TxScrollWindowEx(
        INT dx,
        INT dy,
        LPCRECT lprcScroll,
        LPCRECT lprcClip,
        HRGN hrgnUpdate,
        LPRECT lprcUpdate,
        UINT fuScroll);
    virtual void TxSetCapture(BOOL fCapture);
    virtual void TxSetFocus();
    virtual void TxSetCursor(HCURSOR hcur, BOOL fText);
    virtual BOOL  TxScreenToClient(LPPOINT lppt);
    virtual BOOL  TxClientToScreen(LPPOINT lppt);
    virtual HRESULT  TxActivate(LONG * plOldState);
    virtual HRESULT  TxDeactivate(LONG lNewState);
    virtual HRESULT   TxGetClientRect(LPRECT prc);
    virtual HRESULT  TxGetViewInset(LPRECT prc);
    virtual HRESULT  TxGetCharFormat(const CHARFORMATW ** ppCF );
    virtual HRESULT  TxGetParaFormat(const PARAFORMAT ** ppPF);
    virtual COLORREF  TxGetSysColor(int nIndex);
    virtual HRESULT  TxGetBackStyle(TXTBACKSTYLE * pstyle);
    virtual HRESULT  TxGetMaxLength(DWORD * plength);
    virtual HRESULT  TxGetScrollBars(DWORD * pdwScrollBar);
    virtual HRESULT  TxGetPasswordChar(TCHAR * pch);
    virtual HRESULT  TxGetAcceleratorPos(LONG * pcp);
    virtual HRESULT  TxGetExtent(LPSIZEL lpExtent);
    virtual HRESULT  OnTxCharFormatChange(const CHARFORMATW * pcf);
    virtual HRESULT  OnTxParaFormatChange(const PARAFORMAT * ppf);
    virtual HRESULT  TxGetPropertyBits(DWORD dwMask, DWORD * pdwBits);
    virtual HRESULT  TxNotify(DWORD iNotify, void * pv);
    virtual HIMC  TxImmGetContext();
    virtual void TxImmReleaseContext(HIMC himc);
    virtual HRESULT  TxGetSelectionBarWidth(LONG * plSelBarWidth);

protected:

	BOOL m_fUiActive; /**< Whether control is inplace active */
	BOOL m_bShowCaret;
	ULONG            cRefs;          /**< Reference Count */
	ITextServices *	 m_pITextServices;         /**< pointer to Text Services object */
	IBkRichTextWindow& m_bkWindow;   /**< swindow for text host */
	POINT            m_ptCaret;
	CHARFORMAT2W   m_charFormat;
	PARAFORMAT2   m_paraFormat;
	int			      m_nMaxCharLength;
	DWORD		  m_dwStyle;
	SIZEL			  m_sizelExtent;
	CRect			  m_rcInset;
	int				  m_lAccelPos;
	BkTimer::IBkWndTimer* m_pIBkWndTimer;
	COLORREF      m_crText;
	CRect   m_rcClient;
};

#endif