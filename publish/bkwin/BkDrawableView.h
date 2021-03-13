//********************************************************************
//author:  hexujun@cmcm.com
//create time: 2020/8/4?17:00
//description: ÃèÊö
//*********************************************************************

#ifndef _H_BKWIN_BKDRAWABLEVIEW_H_
#define _H_BKWIN_BKDRAWABLEVIEW_H_

#include "BkRef.h"

class IBkDrawable : public BkRefImpl {
public:
	IBkDrawable() {
	}

	virtual ~IBkDrawable() {
	}

	virtual void Draw(HDC hDC, CBkWindow& pWnd){
		CRect rcRect;
		pWnd.GetRect(rcRect);
		Draw(hDC, rcRect);
	}

	virtual void Draw(HDC hDC, const CRect& rcDest){
	}

	virtual void Draw(Gdiplus::Graphics& graphics, CBkWindow& pWnd){
	}

	virtual CSize CalcSize() {
		return CSize(0, 0);
	}

	virtual int GetWidth() {
		return CalcSize().cx;
	}

	virtual int GetHeight() {
		return CalcSize().cy;
	}

	virtual int GetStatusCount() {
		return 1;
	}
};

class BkDrawableView : public CBkWindow 
{
public:
	BKOBJ_DECLARE_CLASS_NAME(BkDrawableView, "drawableview")

public:
	BkDrawableView() {
	}

	~BkDrawableView() {
	}

	void SetDrawable(IBkDrawable* pDrawable) {
		if (m_drawable == pDrawable) {
			return;
		}

		m_drawable = pDrawable;
		NotifyInvalidate();
	}

	BKWIN_BEGIN_MSG_MAP()
		MSG_WM_PAINT(OnPaint)
		MSG_WM_NCCALCSIZE(OnNcCalcSize)
	BKWIN_END_MSG_MAP()

protected:

	LRESULT OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam) {
		__super::OnNcCalcSize(bCalcValidRects, lParam);
		LPSIZE pSize = (LPSIZE)lParam;
		if(m_dlgpos.nCount == 4) {
			return S_OK;
		}

		if(m_drawable) {
			*pSize = m_drawable->CalcSize();
		}

		return S_OK;
	}

	void OnPaint(CDCHandle dc) 
	{
		if (m_drawable) {
			m_drawable->Draw(dc, *this);
		}
	}

protected:
	virtual BOOL NeedRedrawWhenStateChange()
	{
		if(m_drawable && m_drawable->GetStatusCount() > 1) {
			return TRUE;
		}
		return __super::NeedRedrawWhenStateChange();
	}

protected:
	BkSafePtr<IBkDrawable> m_drawable;
};

#endif