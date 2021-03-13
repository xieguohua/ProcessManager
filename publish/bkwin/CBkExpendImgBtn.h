#pragma once

#include "bkwin/bkdlgview.h"
#include <math.h>
#include "CBkMsgWnd.h"
#include "BkwndExDef.h"
#include "CBkDrawShadowText.h"
#include "bkwndcmnctrl.h"
#include "bkskinex.h"

class SudokuDraw
{
public:
	struct SudokuParam
	{
		int m_nSkinAlpha;
		int m_nMarginLeft;
		int m_nMarginRight;
		int m_nMarginTop;
		int m_nMarginBottom;
		BOOL m_bStretch;
	
		int m_nDrawWidth;
		int m_nDrawHeight;
		int m_nSrcPosX;
		int m_nSrcPosY;

		CRect m_rcDrawArea;

		SudokuParam()
		{
			m_nSkinAlpha = 255;
			m_nMarginLeft = 0;
			m_nMarginRight = 0;
			m_nMarginTop = 0;
			m_nMarginBottom = 0;
			m_bStretch = FALSE;

			m_nDrawWidth = 0;
			m_nDrawHeight = 0;
			m_nSrcPosX = 0;
			m_nSrcPosY = 0;
		}
	};

	void Draw(CDCHandle& dc, CBkSkinBase* pSkin, SudokuParam& sudokuParam)
	{
		if (pSkin == NULL)
		{
			return;
		}

		if (sudokuParam.m_nDrawWidth == 0 || sudokuParam.m_nDrawHeight == 0)
		{
			return;
		}

		CDC			dcMem;
		CBkBitmap	bmpMem;
		CRect		rcDrawArea(0, 0, sudokuParam.m_rcDrawArea.Width(), sudokuParam.m_rcDrawArea.Height());

		dcMem.CreateCompatibleDC(dc);
		bmpMem.CreateDIBSection(sudokuParam.m_rcDrawArea.Width(), sudokuParam.m_rcDrawArea.Height());
		HBITMAP hOldBitmap = dcMem.SelectBitmap(bmpMem);

		CBkBitmap bmpCache;
		CDCHandle dcMemHandle;
		CRect	  rcDraw(rcDrawArea);
		CRect	  rcSrc(0, 0, sudokuParam.m_nMarginLeft, sudokuParam.m_nMarginTop);

		dcMemHandle.Attach(dcMem);
		bmpCache.CreateDIBSection(sudokuParam.m_nDrawWidth, sudokuParam.m_nDrawHeight);

		{
			CDC dcMem2;
			CDCHandle dcMemHandle2;
			CRect rcDrawSrc(sudokuParam.m_nSrcPosX, 
				sudokuParam.m_nSrcPosY, 
				sudokuParam.m_nSrcPosX + sudokuParam.m_nDrawWidth, 
				sudokuParam.m_nSrcPosY + sudokuParam.m_nDrawHeight);
			CRect rcDrawDes(0, 0, sudokuParam.m_nDrawWidth, sudokuParam.m_nDrawHeight);

			dcMem2.CreateCompatibleDC(dc);				
			dcMemHandle2.Attach(dcMem2);
			HBITMAP hOldBitmap2 = dcMem2.SelectBitmap(bmpCache);
			pSkin->DrawImage(dcMemHandle2, rcDrawDes, rcDrawSrc);
			dcMem2.SelectBitmap(hOldBitmap2);
		}

		//»æÖÆ×óÉÏ½Ç
		if (sudokuParam.m_nMarginLeft && sudokuParam.m_nMarginTop)
		{
			CBkBitmap::AlphaBlend(
				dcMemHandle, 
				rcDraw.left, 
				rcDraw.top, 
				sudokuParam.m_nMarginLeft, 
				sudokuParam.m_nMarginTop, 
				bmpCache, 
				0, 
				0, 
				sudokuParam.m_nMarginLeft, 
				sudokuParam.m_nMarginTop,
				255);
		}
		//»æÖÆÓÒÉÏ½Ç
		if (sudokuParam.m_nMarginRight && sudokuParam.m_nMarginTop)
		{
			CBkBitmap::AlphaBlend(
				dcMemHandle, 
				rcDraw.right - sudokuParam.m_nMarginRight, 
				rcDraw.top, 
				sudokuParam.m_nMarginRight, 
				sudokuParam.m_nMarginTop,
				bmpCache, 
				sudokuParam.m_nDrawWidth - sudokuParam.m_nMarginRight, 
				0, 
				sudokuParam.m_nMarginRight, 
				sudokuParam.m_nMarginTop,
				255);
		}
		//»æÖÆ×óÏÂ½Ç
		if (sudokuParam.m_nMarginLeft && sudokuParam.m_nMarginBottom)
		{
			CBkBitmap::AlphaBlend(
				dcMemHandle, 
				rcDraw.left, 
				rcDraw.bottom - sudokuParam.m_nMarginBottom, 
				sudokuParam.m_nMarginLeft, 
				sudokuParam.m_nMarginBottom,
				bmpCache, 
				0, 
				sudokuParam.m_nDrawHeight - sudokuParam.m_nMarginBottom, 
				sudokuParam.m_nMarginLeft, 
				sudokuParam.m_nMarginBottom,
				255);
		}
		//»æÖÆÓÒÏÂ½Ç
		if (sudokuParam.m_nMarginRight && sudokuParam.m_nMarginBottom)
		{
			CBkBitmap::AlphaBlend(
				dcMemHandle, 
				rcDraw.right - sudokuParam.m_nMarginRight, 
				rcDraw.bottom - sudokuParam.m_nMarginBottom, 
				sudokuParam.m_nMarginRight, 
				sudokuParam.m_nMarginBottom,
				bmpCache, 
				sudokuParam.m_nDrawWidth - sudokuParam.m_nMarginRight, 
				sudokuParam.m_nDrawHeight - sudokuParam.m_nMarginBottom, 
				sudokuParam.m_nMarginRight, 
				sudokuParam.m_nMarginBottom, 
				255);
		}
		//»æÖÆ×ó±ß
		if (sudokuParam.m_nMarginLeft)
		{
			CBkBitmap::AlphaBlend(
				dcMemHandle, 
				rcDraw.left, 
				rcDraw.top + sudokuParam.m_nMarginTop, 
				sudokuParam.m_nMarginLeft, 
				rcDraw.Height() - sudokuParam.m_nMarginTop - sudokuParam.m_nMarginBottom,
				bmpCache, 
				0, 
				sudokuParam.m_nMarginTop, 
				sudokuParam.m_nMarginLeft, 
				(sudokuParam.m_nDrawHeight < rcDraw.Height() ? sudokuParam.m_nDrawHeight : rcDraw.Height()) - sudokuParam.m_nMarginTop - sudokuParam.m_nMarginBottom, 
				255);
		}
		//»æÖÆÓÒ±ß
		if (sudokuParam.m_nMarginRight)
		{
			CBkBitmap::AlphaBlend(
				dcMemHandle, 
				rcDraw.right - sudokuParam.m_nMarginRight, 
				rcDraw.top + sudokuParam.m_nMarginTop, 
				sudokuParam.m_nMarginRight, 
				rcDraw.Height() - sudokuParam.m_nMarginTop - sudokuParam.m_nMarginBottom,
				bmpCache, 
				sudokuParam.m_nDrawWidth - sudokuParam.m_nMarginRight, 
				sudokuParam.m_nMarginTop, 
				sudokuParam.m_nMarginRight, 
				(sudokuParam.m_nDrawHeight < rcDraw.Height() ? sudokuParam.m_nDrawHeight : rcDraw.Height()) - sudokuParam.m_nMarginTop - sudokuParam.m_nMarginBottom, 
				255);
		}
		//»æÖÆ¶¥²¿
		if (sudokuParam.m_nMarginTop)
		{
			CBkBitmap::AlphaBlend(
				dcMemHandle, 
				rcDraw.left + sudokuParam.m_nMarginLeft, 
				rcDraw.top, 
				rcDraw.Width() - sudokuParam.m_nMarginLeft - sudokuParam.m_nMarginRight, 
				sudokuParam.m_nMarginTop,
				bmpCache, 
				sudokuParam.m_nMarginLeft, 
				0, 
				(sudokuParam.m_nDrawWidth < rcDraw.Width() ? sudokuParam.m_nDrawWidth : rcDraw.Width()) - sudokuParam.m_nMarginLeft - sudokuParam.m_nMarginRight, 
				sudokuParam.m_nMarginTop, 
				255);
		}
		//»æÖÆµ×²¿
		if (sudokuParam.m_nMarginBottom)
		{
			CBkBitmap::AlphaBlend(
				dcMemHandle, 
				rcDraw.left + sudokuParam.m_nMarginLeft, 
				rcDraw.bottom - sudokuParam.m_nMarginBottom, 
				rcDraw.Width() - sudokuParam.m_nMarginLeft - sudokuParam.m_nMarginRight, 
				sudokuParam.m_nMarginBottom,
				bmpCache, 
				sudokuParam.m_nMarginLeft, 
				sudokuParam.m_nDrawHeight - sudokuParam.m_nMarginBottom, 
				(sudokuParam.m_nDrawWidth < rcDraw.Width() ? sudokuParam.m_nDrawWidth : rcDraw.Width()) - sudokuParam.m_nMarginLeft - sudokuParam.m_nMarginRight, 
				sudokuParam.m_nMarginBottom, 
				255);
		}

		//»æÖÆÖÐ¼ä
		if (!sudokuParam.m_bStretch)
		{
			CBkBitmap::AlphaBlend(
				dcMemHandle, 
				rcDraw.left + sudokuParam.m_nMarginLeft, 
				rcDraw.top +sudokuParam. m_nMarginTop, 
				rcDraw.Width() - sudokuParam.m_nMarginLeft - sudokuParam.m_nMarginRight, 
				rcDraw.Height() - sudokuParam.m_nMarginTop - sudokuParam.m_nMarginBottom,
				bmpCache, 
				sudokuParam.m_nMarginLeft, 
				sudokuParam.m_nMarginTop, 
				(sudokuParam.m_nDrawWidth < rcDraw.Width() ? sudokuParam.m_nDrawWidth : rcDraw.Width()) - sudokuParam.m_nMarginLeft - sudokuParam.m_nMarginRight, 
				(sudokuParam.m_nDrawHeight < rcDraw.Height() ? sudokuParam.m_nDrawHeight : rcDraw.Height()) - sudokuParam.m_nMarginTop - sudokuParam.m_nMarginBottom, 
				255);
		}
		else
		{
			CBkBitmap::AlphaBlend(
				dcMemHandle, 
				rcDraw.left + sudokuParam.m_nMarginLeft, 
				rcDraw.top + sudokuParam.m_nMarginTop, 
				rcDraw.Width() - sudokuParam.m_nMarginLeft - sudokuParam.m_nMarginRight, 
				rcDraw.Height() - sudokuParam.m_nMarginTop - sudokuParam.m_nMarginBottom,
				bmpCache, 
				sudokuParam.m_nMarginLeft, 
				sudokuParam.m_nMarginTop, 
				sudokuParam.m_nDrawWidth - sudokuParam.m_nMarginLeft - sudokuParam.m_nMarginRight, 
				sudokuParam.m_nDrawHeight - sudokuParam.m_nMarginTop - sudokuParam.m_nMarginBottom, 
				255);
		}

		dcMem.SelectBitmap(hOldBitmap);

		CBkBitmap::AlphaBlend(dc, 
			sudokuParam.m_rcDrawArea.left, 
			sudokuParam.m_rcDrawArea.top, 
			sudokuParam.m_rcDrawArea.Width(), 
			sudokuParam.m_rcDrawArea.Height(),
			bmpMem, 0, 0, bmpMem.GetWidth(), bmpMem.GetHeight(), sudokuParam.m_nSkinAlpha);
	}
};

class CBkExpendImgBtn : public CBkImageBtnWnd
{
public:
	BKOBJ_DECLARE_CLASS_NAME(CBkExpendImgBtn, "expendimgbtn")

	CBkExpendImgBtn()
	{
		m_nSkinAlpha = 255;
		m_nMarginLeft = 0;
		m_nMarginRight = 0;
		m_nMarginTop = 0;
		m_nMarginBottom = 0;
		m_bStretch = TRUE;
	}

protected:

	void OnPaint(CDCHandle dc)
	{
		SetMsgHandled(TRUE);

		BkDCPaint bkdc;
		BeforePaint(dc, bkdc);

		CBkSkinBase *pSkin = BkSkin::GetSkin(m_strSkinName);
		if (pSkin == NULL ||
			m_rcWindow.Width() == 0 ||
			m_rcWindow.Height() == 0)
		{
			return;
		}

		DWORD dwState = IIF_STATE4(GetState(), 0, 1, 2, 3);

		int nDrawWidth = 0, nDrawHeight = 0, nSrcPosX = 0, nSrcPosY = 0;
		CSize szSkin;
		szSkin = pSkin->GetSkinSize();

		nDrawHeight = szSkin.cy;

		if (-1 == dwState)
		{
			nDrawWidth = szSkin.cx;
		}
		else
		{
			nDrawWidth = szSkin.cx;
			nSrcPosX = dwState * szSkin.cx;
		}

        
		/*SudokuDraw::SudokuParam param;
		param.m_bStretch = m_bStretch;
		param.m_nSrcPosX = nSrcPosX;
		param.m_nSrcPosY = nSrcPosY;
		param.m_nDrawHeight = nDrawHeight;
		param.m_nDrawWidth = nDrawWidth;
		param.m_nMarginBottom = m_nMarginBottom;
		param.m_nMarginLeft = m_nMarginLeft;
		param.m_nMarginRight = m_nMarginRight;
		param.m_nMarginTop = m_nMarginTop;
		param.m_nSkinAlpha = m_nSkinAlpha;
		param.m_rcDrawArea = m_rcWindow;

		m_sudukuDraw.Draw(dc, pSkin, param);*/

        CRect rcCel(m_nMarginLeft, m_nMarginTop, m_nMarginRight, m_nMarginBottom);
        pSkin->DrawNineCels(dc, dwState, m_rcWindow, rcCel, m_nSkinAlpha);

        if (!m_strInnerText.IsEmpty())
        {
            if (0 == m_nTextAlign) 
                m_nTextAlign = GetStyle().m_nTextAlign;

            if (BkFontPool::IsYaHei())
            {
                int nAddSize = BkFontPool::GetFontSizeAdding(dc.GetCurrentFont());

                if ((m_nTextAlign & DT_BOTTOM) == DT_BOTTOM)
                {
                    int nTop = bkdc.rcClient.top;
                    bkdc.rcClient.top -= 2 * (nAddSize + 1); 
                    bkdc.rcClient.bottom -= 2 * (nAddSize + 1);
                    int nMinHeight = BkFontPool::GetDefaultFontSize() + nAddSize + 2 * (nAddSize + 2);
                    if (bkdc.rcClient.Height() < nMinHeight)
                        bkdc.rcClient.bottom += nMinHeight - bkdc.rcClient.Height();


                    if (bkdc.rcClient.bottom - 3 - (BkFontPool::GetDefaultFontSize() + nAddSize) < nTop)
                        bkdc.rcClient.bottom = nTop + BkFontPool::GetDefaultFontSize() + nAddSize + 3;
                }
                else if ((m_nTextAlign & DT_VCENTER) == DT_VCENTER)
                {
                    bkdc.rcClient.top -= 0; 
                    bkdc.rcClient.bottom -= 0;
                }
                else
                {
                    bkdc.rcClient.top -= 3 * (nAddSize + 1); 
                    bkdc.rcClient.bottom -= 3 * (nAddSize + 1);
                }
            }

            if (GetStyle().m_nGdiplus)
            {
                Gdiplus::Graphics graphics( dc );
                BkGdiplusFont font(dc, dc.GetCurrentFont());

                Gdiplus::SolidBrush blackBrush(Gdiplus::Color(254, GetRValue(dc.GetTextColor()), GetGValue(dc.GetTextColor()), GetBValue(dc.GetTextColor())));

                Gdiplus::StringFormat format;
                if ((m_nTextAlign & DT_CENTER) == DT_CENTER)
                    format.SetAlignment(Gdiplus::StringAlignmentCenter);

                if ((m_nTextAlign & DT_VCENTER) == DT_VCENTER)
                    format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

                if ((m_nTextAlign & DT_END_ELLIPSIS) == DT_END_ELLIPSIS)
                    format.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
                if ((m_nTextAlign & DT_WORD_ELLIPSIS) == DT_WORD_ELLIPSIS)
                    format.SetTrimming(Gdiplus::StringTrimmingEllipsisWord);
                if ((m_nTextAlign & DT_PATH_ELLIPSIS) == DT_PATH_ELLIPSIS)
                    format.SetTrimming(Gdiplus::StringTrimmingEllipsisPath);

                Gdiplus::RectF layoutRect(bkdc.rcClient.left, bkdc.rcClient.top, bkdc.rcClient.Width(), bkdc.rcClient.Height());

                graphics.DrawString(
                    m_strInnerText,
                    -1,
                    font,
                    layoutRect,
                    &format,
                    &blackBrush);
            }
            else
            {
                int nRet = 0;
                if (GetStyle().m_nShadow != 0)
                {
                    nRet = dc.DrawShadowText(
                        m_strInnerText, 
                        m_strInnerText.GetLength(), 
                        bkdc.rcClient, 
                        m_nTextAlign, 
                        dc.GetTextColor(), 
                        GetStyle().m_crShadow, 
                        2, 
                        2);
                }

                if (0 == nRet)
                    dc.DrawText(
                    m_strInnerText, m_strInnerText.GetLength(), 
                    bkdc.rcClient, 
                    m_nTextAlign);
            }
        }

		AfterPaint(dc, bkdc);
	}

	LRESULT OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam)
	{
		SetMsgHandled(TRUE);

		return CBkWindow::OnNcCalcSize(bCalcValidRects, lParam);
	}

	BKWIN_DECLARE_ATTRIBUTES_BEGIN()
		BKWIN_INT_ATTRIBUTE("skinalpha", m_nSkinAlpha, TRUE)//, 0)
		BKWIN_INT_ATTRIBUTE("margin_left", m_nMarginLeft, TRUE)//, 0)
		BKWIN_INT_ATTRIBUTE("margin_right", m_nMarginRight, TRUE)//, 0)
		BKWIN_INT_ATTRIBUTE("margin_top", m_nMarginTop, TRUE)//, 0)
		BKWIN_INT_ATTRIBUTE("margin_bottom", m_nMarginBottom, TRUE)//, 0)
		BKWIN_INT_ATTRIBUTE("stretch", m_bStretch, TRUE)//, 0)
	BKWIN_DECLARE_ATTRIBUTES_END()

	BKWIN_BEGIN_MSG_MAP()
		MSG_WM_PAINT(OnPaint)
		MSG_WM_NCCALCSIZE(OnNcCalcSize)
	BKWIN_END_MSG_MAP()

protected:

	int		 m_nSkinAlpha;
	int		 m_nMarginLeft;
	int		 m_nMarginRight;
	int		 m_nMarginTop;
	int		 m_nMarginBottom;
	BOOL     m_bStretch;
	CStringA m_strSkinA;

	SudokuDraw m_sudukuDraw;
};

class CBkExpendImgEx : public CBkExpendImg
{
public:
	BKOBJ_DECLARE_CLASS_NAME(CBkExpendImgEx, "expendimgex")

	CBkExpendImgEx()
	{
		m_nDrawSrcX = 0;
		m_nDrawSrcY = 0;
		m_nDrawSrcWidth = 0;
		m_nDrawSrcHeight = 0;
	}

protected:

	void OnPaint(CDCHandle dc)
	{
		SetMsgHandled(TRUE);
	
		CBkSkinBase* pSkin = BkSkin::GetSkin(m_strSkinA);
		if (pSkin == NULL ||
			m_rcWindow.Width() == 0 ||
			m_rcWindow.Height() == 0)
		{
			return;
		}

		CSize szImg = pSkin->GetSkinSize();
		if (m_nDrawSrcHeight == 0)
		{
			m_nDrawSrcHeight = szImg.cy;
		}

		if (m_nDrawSrcWidth == 0)
		{
			m_nDrawSrcWidth = szImg.cx;
		}

		SudokuDraw::SudokuParam param;
		param.m_bStretch = m_bStretch;
		param.m_nSrcPosX = m_nDrawSrcX;
		param.m_nSrcPosY = m_nDrawSrcY;
		param.m_nDrawHeight = m_nDrawSrcHeight;
		param.m_nDrawWidth = m_nDrawSrcWidth;
		param.m_nMarginBottom = m_nMarginBottom;
		param.m_nMarginLeft = m_nMarginLeft;
		param.m_nMarginRight = m_nMarginRight;
		param.m_nMarginTop = m_nMarginTop;
		param.m_nSkinAlpha = m_nSkinAlpha;
		param.m_rcDrawArea = m_rcWindow;

		m_sudukuDraw.Draw(dc, pSkin, param);
	}

	BKWIN_DECLARE_ATTRIBUTES_BEGIN()
		BKWIN_INT_ATTRIBUTE("srcx", m_nDrawSrcX, TRUE)//, 0)
		BKWIN_INT_ATTRIBUTE("srcy", m_nDrawSrcY, TRUE)//, 0)
		BKWIN_INT_ATTRIBUTE("srcwidth", m_nDrawSrcWidth, TRUE)//, 0)
		BKWIN_INT_ATTRIBUTE("srcheight", m_nDrawSrcHeight, TRUE)//, 0)
	BKWIN_DECLARE_ATTRIBUTES_END()

	BKWIN_BEGIN_MSG_MAP()
		MSG_WM_PAINT(OnPaint)
	BKWIN_END_MSG_MAP()

private:
	int m_nDrawSrcX;
	int m_nDrawSrcY;
	int m_nDrawSrcWidth;
	int m_nDrawSrcHeight;

	SudokuDraw m_sudukuDraw;
};