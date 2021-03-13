/*************************************************
//  File name:       bkshadowdrawable.h
//  Date:              2020/11/17
//  <author>      hexujun@cmcm.com
//  <time>          2020/11/17
//   <version>     1.0    
*************************************************/
#ifndef _bkshadowdrawable_h_
#define _bkshadowdrawable_h_

#include "BkRef.h"
#include "bkbitmap.h"
#include "bkshadowwnd.h"

namespace BkWin {\

class BkShadowDrawable : public BkRefImpl
{
public:

	BkShadowDrawable(const CRect& rcMargin)
	{
		InitMember();
		SetShadowMargin(rcMargin);
	}

	BkShadowDrawable()
	{
		InitMember();
	}

   ~BkShadowDrawable(void)
	{
		m_bmpShadow.DeleteObject();
	}

	void SetShadowMargin(const CRect& rcMargin)
	{
		if (m_rcShadowMargin == rcMargin) 
		{
			return;
		}

		m_rcShadowMargin = rcMargin;
	}

	virtual void Draw(HDC hDC, const CRect& rcDest)
	{
		if (!m_bmpShadow) 
		{
			return;
		}

		CBkBitmap::AlphaBlendNineCels(hDC, rcDest, m_bmpShadow, CRect(0, 0, m_bmpShadow.GetWidth(), m_bmpShadow.GetHeight()), m_rcShadowMargin, 255);
	}

	void SetPngResID(UINT UResID)
	{
		Gdiplus::Image* pImage = BkPngPool::Get(UResID);
		if (pImage == NULL) {
			m_bmpShadow.DeleteObject();
			return;
		}
		m_bmpShadow.CreateFromImage(pImage);
	}

	void SetSkinName(LPCSTR lpszSkinName) {
		CBkSkinBase* pSkin = BkSkin::GetSkin(lpszSkinName);
		if (pSkin == NULL) {
			m_bmpShadow.DeleteObject();
			return;
		}

		m_bmpShadow.CreateDIBSection(pSkin->GetSkinSize().cx, pSkin->GetSkinSize().cy);
		if(!m_bmpShadow) {
			return;
		}

		{
			CDC dcMem;
			CDCHandle dcDesktop(::GetDC(NULL));
			dcMem.CreateCompatibleDC(NULL);
			::ReleaseDC(NULL, dcDesktop);

			HBITMAP hBmpOld = dcMem.SelectBitmap(m_bmpShadow);
			pSkin->Draw(dcMem.m_hDC, CRect(0, 0, m_bmpShadow.GetWidth(), m_bmpShadow.GetHeight()), 0);
			dcMem.SelectBitmap(hBmpOld);
		}
	}

	CSize GetSize() const {
		return CSize(m_bmpShadow.GetWidth(), m_bmpShadow.GetHeight());
	}

protected:
	void InitMember()
	{
		m_rcShadowMargin.SetRectEmpty();
	}

protected:
	CRect m_rcShadowMargin;
	CBkBitmap m_bmpShadow;
};

template<class T>
class WndShadowHelper  : public IShadowPaintHook
{
public:
	WndShadowHelper() {
		T* pThis = static_cast<T*>(this);
		pThis->SetDrawBorder(FALSE);
		pThis->SetShadowPaintHook(this);
		pThis->EnableShadow(TRUE);
	}

public:
	
	virtual BOOL NotifyShadowPaint(HDC hDC, CRect& rct) 
	{
		if (m_shadowDrawable) {
			m_shadowDrawable->Draw(hDC, rct);
		}
		return FALSE;
	}

	void SetShadowData(int nShdaowSize, UINT uPngResID) {
			T* pThis = static_cast<T*>(this);
			pThis->SetShadowSize(nShdaowSize);

			m_shadowDrawable = BkSafePtr<BkShadowDrawable>(new BkShadowDrawable(), false);
			m_shadowDrawable->SetPngResID(uPngResID);
			UpdateShadowMargin();
	}

	void SetShadowData(int nShdaowSize, LPCSTR lpszSkinName) {
		T* pThis = static_cast<T*>(this);
		pThis->SetShadowSize(nShdaowSize);

		m_shadowDrawable = BkSafePtr<BkShadowDrawable>(new BkShadowDrawable(), false);
		m_shadowDrawable->SetSkinName(lpszSkinName);
		UpdateShadowMargin();
	}

	void SetShadowDrawable(BkSafePtr<BkShadowDrawable>& drawable) {
		m_shadowDrawable = drawable;
	}

private:
	void UpdateShadowMargin() {
		if(!m_shadowDrawable) {
			return;
		}

		CSize sizeDrawable = m_shadowDrawable->GetSize();
		CRect rcMargin;

		rcMargin.left = sizeDrawable.cx / 2;
		rcMargin.right = sizeDrawable.cx - rcMargin.left - 1;
		rcMargin.top = sizeDrawable.cy / 2;
		rcMargin.bottom = sizeDrawable.cy - rcMargin.top - 1;

		m_shadowDrawable->SetShadowMargin(rcMargin);
	}

protected:
	BkSafePtr<BkShadowDrawable> m_shadowDrawable;
};

}
#endif