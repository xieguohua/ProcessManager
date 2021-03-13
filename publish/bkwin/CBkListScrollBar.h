
#pragma once

#include <math.h>

#pragma warning(disable:4244)

class CBkWindowScollBar : public CBkWindow
{
public:

	BKOBJ_DECLARE_CLASS_NAME(CBkWindowScollBar, "listscrollbar")

	CBkWindowScollBar()
	{
		m_nPos = 0;
		m_nMaxPos = 0;
		m_nViewSize = 0;
		m_nPicSize = 0;
        m_nBarWidth = 11;
		m_nBarHeight = 11;
		m_bPermanentHide = FALSE;
		m_bHorizontalBar = FALSE;

		m_strSkinBar = "bk_scroll_bar";
		m_strSkinBg = "bk_scroll_bg";
		m_dwCurItem = 0;
        m_bRectangleBar = FALSE;

		m_color.SetValue(Gdiplus::Color::MakeARGB(50, 0, 0, 0));
		m_colorHover.SetValue(Gdiplus::Color::MakeARGB(100, 0, 0, 0));
		m_colorBorder.SetValue(0);
	}

	int GetScrollPos()
	{
		return m_nPos;
	}

	void SetScrollPos(int nPos)
	{
		if (nPos >= 0 && nPos <= m_nMaxPos)
		{
			m_nPos = nPos;
			ReCalcBarPos();
		}
	}

	void SetHorizontalBar(BOOL bHorizontalBar)
	{
		m_bHorizontalBar = bHorizontalBar;
	}

	void SetShowBar(BOOL bShow)
	{
		m_bPermanentHide = !bShow;
	}

	int GetMaxScrollPos()
	{
		return m_nMaxPos;
	}

	int GetPic()
	{
		return m_nPicSize;
	}

	int GetViewSize()
	{
		return m_nViewSize;
	}


	virtual void SetScrollRange(int nView, int nPic)
	{
		m_nViewSize = nView;
		m_nPicSize = nPic;

		m_nMaxPos = nPic - nView;

		if (m_nPos > m_nMaxPos)
		{
			m_nPos = m_nMaxPos;
		}

		if (m_nPos < 0)
		{
			m_nPos = 0;
		}

		if (m_nMaxPos <= 0)
		{
			m_nMaxPos = 0;
			SetVisible(FALSE);
		}
		else 
		{
			if (m_nPos > m_nMaxPos)
			{
				m_nPos = m_nMaxPos;
			}

			SetVisible(TRUE);
			ReCalcBarPos();
		}
	}

	//view 的滚动
	virtual void ScrollOffset(int nOffset)
	{
		if(m_bHorizontalBar)
		{
			ScrollOffsetHorizontal(nOffset);
		}
		else
		{
			ScrollOffsetVertical(nOffset);
		}
	}

	
	virtual void ScrollOffsetHorizontal(int nOffset)
	{
		if (nOffset != 0)
		{
			if (m_rcBar.left + nOffset < m_rcWindow.left)
			{
				nOffset = -(m_rcBar.left - m_rcWindow.left);
			}
			else if (m_rcBar.right + nOffset > m_rcWindow.right)
			{
				nOffset = m_rcWindow.right - m_rcBar.right;
			}

			int nOldPos = m_nPos;
			m_nPos += ViewToPos(nOffset);

			if (m_nPos > m_nMaxPos)
				m_nPos = m_nMaxPos;

			if (m_nPos < 0)
				m_nPos = 0;

			int nLeft = PosToView(m_nPos);
			int nWidth = m_rcBar.Width();
			m_rcBar.left = nLeft;
			m_rcBar.right = m_rcBar.left + nWidth;

			//SetScrollPos(m_nPos);

			CBkWindow* pWndParent = BkWnds::GetWindow(m_hBkWndParent);
			if (pWndParent)
			{
				pWndParent->BkSendMessage(WM_BK_LIST_OFFSET, m_nPos - nOldPos, 0);
			}
		}
	}

	virtual void ScrollOffsetVertical(int nOffset)
	{
		if (nOffset != 0)
		{
			if (m_rcBar.top + nOffset < m_rcWindow.top)
			{
				nOffset = -(m_rcBar.top - m_rcWindow.top);
			}
			else if (m_rcBar.bottom + nOffset > m_rcWindow.bottom)
			{
				nOffset = m_rcWindow.bottom - m_rcBar.bottom;
			}

			int nOldPos = m_nPos;
			m_nPos += ViewToPos(nOffset);

			if (m_nPos > m_nMaxPos)
				m_nPos = m_nMaxPos;

			if (m_nPos < 0)
				m_nPos = 0;

			int nTop = PosToView(m_nPos);
			int nHeight = m_rcBar.Height();
			m_rcBar.top = nTop;
			m_rcBar.bottom = m_rcBar.top + nHeight;

			//SetScrollPos(m_nPos);

			CBkWindow* pWndParent = BkWnds::GetWindow(m_hBkWndParent);
			if (pWndParent)
			{
				pWndParent->BkSendMessage(WM_BK_LIST_OFFSET, m_nPos - nOldPos, 0);
			}
		}
	}

	virtual BOOL RedrawRegion(CDCHandle& dc, CRgn& rgn)
	{
		if (m_bPermanentHide)
		{
			return FALSE;
		}

		if (rgn.IsNull() || rgn.RectInRegion(m_rcWindow))
		{
			BkDCPaint BkDC;

			BeforePaint(dc, BkDC);
			BkSendMessage(WM_PAINT, (WPARAM)(dc.m_hDC));
			AfterPaint(dc, BkDC);

			return TRUE;
		}

		return FALSE;
	}

	virtual BOOL NeedRedrawWhenStateChange()
	{
		return TRUE;
	}

	virtual HBKWND BkGetHWNDFromPoint(POINT ptHitTest, BOOL bOnlyText)
	{
		if (!m_bPermanentHide && m_rcValidte.PtInRect(ptHitTest))
		{
			return m_hBkWnd;
		}

		return NULL;
	}

protected:

	virtual void ReCalcBarPos()
	{
		if (!IsVisible() || m_nPicSize == 0)
		{
			return;
		}
		
		if(m_bHorizontalBar)
		{
			ReCalcBarPosHorizontal();
		}
		else
		{
			ReCalcBarPosVertical();
		}

	}

	virtual void ReCalcBarPosHorizontal()
	{
		int nTotalPos = m_rcWindow.Width();

		m_nBarLength = nTotalPos * m_nViewSize / m_nPicSize;
		if (m_nBarLength < 20)
		{
			m_nBarLength = 20;
		}

		//根据当前的nscrollPos计算bar应该在的位置
		m_rcBar.left = PosToView(m_nPos);
		m_rcBar.right = m_rcBar.left + m_nBarLength;
	}

	virtual void ReCalcBarPosVertical()
	{
		int nTotalPos = m_rcWindow.Height();

		m_nBarLength = nTotalPos * m_nViewSize / m_nPicSize;
		if (m_nBarLength < 20)
		{
			m_nBarLength = 20;
		}

		//根据当前的nscrollPos计算bar应该在的位置
		m_rcBar.top = PosToView(m_nPos);
		m_rcBar.bottom = m_rcBar.top + m_nBarLength;
	}

	virtual int ViewToPos(int nView)
	{
		int nLength = m_bHorizontalBar ? m_rcWindow.Width() : m_rcWindow.Height();
		double dl = nLength - m_nBarLength;

		if (dl == 0)
		{
			return 0;
		}

		double dr = m_nMaxPos * nView / dl;

		if (dr > 0)
		{
			return ceil(dr);
		}
		else
		{
			return floor(dr);
		}
	}

	virtual int PosToView(int nPos)
	{
		if (m_nMaxPos == 0)
		{
			return 0;
		}
		int nLength = m_bHorizontalBar ? m_rcWindow.Width() : m_rcWindow.Height();
		double dl = nLength - m_nBarLength;
		double dr = nPos * dl / m_nMaxPos;

		if (dr > 0)
		{
			return ceil(dr);
		}
		else
		{
			return floor(dr);
		}
	}

	void OutputDebugString(LPCWSTR szLog)
	{
		CString strLog(szLog);
		SYSTEMTIME sysTime;

		GetLocalTime(&sysTime);
		strLog.AppendFormat(L"{ %02d %02d %02d %03d }\n", sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);

		::OutputDebugString(strLog);
	}


protected:

	void OnPaint(CDCHandle dc)
	{
		CRect rcDraw(m_rcBar);
		BOOL  bHover = (((GetState() & BkWndState_Hover) || (GetState() & BkWndState_PushDown)) ? TRUE : FALSE);

		int nPenWdith = m_bHorizontalBar ? rcDraw.Height(): rcDraw.Width();
		
		Gdiplus::Color crPen(bHover ? m_colorHover : m_color);
		Gdiplus::Pen penDraw(crPen, nPenWdith);
        

        if (!m_bRectangleBar)
        {
            penDraw.SetEndCap(Gdiplus::LineCapRound);
            penDraw.SetStartCap(Gdiplus::LineCapRound);
        }
		
		penDraw.SetAlignment(Gdiplus::PenAlignmentCenter);

		Gdiplus::Graphics graphics(dc.m_hDC);

		graphics.SetSmoothingMode (Gdiplus::SmoothingModeAntiAlias );

		if(m_bHorizontalBar)
		{
			if (rcDraw.left <= m_rcValidte.left)
			{
				rcDraw.left = m_rcValidte.left + 2;
				rcDraw.right += 2;
			}

			if (rcDraw.right >= m_rcValidte.right)
			{
				rcDraw.right = m_rcValidte.right - 2;
			}
			int nMidPosY = rcDraw.top + nPenWdith / 2;
			graphics.DrawLine(&penDraw, 
				Gdiplus::PointF(rcDraw.left + nPenWdith / 2, nMidPosY), 
				Gdiplus::PointF(rcDraw.right - nPenWdith / 2 - 1, nMidPosY));
		}
		else
		{
			if (rcDraw.top <= m_rcValidte.top)
			{
				rcDraw.top = m_rcValidte.top + 2;
				rcDraw.bottom += 2;
			}

			if (rcDraw.bottom >= m_rcValidte.bottom)
			{
				rcDraw.bottom = m_rcValidte.bottom - 2;
			}
			int nMidPosX = rcDraw.left + nPenWdith / 2;
			graphics.DrawLine(&penDraw, 
				Gdiplus::PointF(nMidPosX, rcDraw.top + nPenWdith / 2), 
				Gdiplus::PointF(nMidPosX, rcDraw.bottom - nPenWdith / 2 - 1));
		}


		if (m_colorBorder.GetAlpha() > 0)
		{
			Gdiplus::Pen penBorder(m_colorBorder);

			if (m_bRectangleBar)
			{
				graphics.DrawRectangle(&penBorder, rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height());
			}
			else
			{
				int nRoundWidht =  rcDraw.Width() / 2;
				int nWidth = nRoundWidht * 2;
				int nHeight = nRoundWidht * 2;

				INT left = rcDraw.left;
				INT right = rcDraw.right;
				INT top = rcDraw.top;
				INT bottom = rcDraw.bottom;
				INT offsetX = nRoundWidht * 2;
				INT offsetY = nRoundWidht * 2;
				int d = nRoundWidht;

				Gdiplus::GraphicsPath path;

				path.AddArc(right - offsetX - 1, top, offsetX, offsetY, 270, 90);
				path.AddArc(right-offsetX - 1, bottom-offsetY - 1, offsetX, offsetY, 0, 90);
				path.AddArc(left, bottom - offsetY - 1, offsetX, offsetY, 90, 90);
				path.AddArc(left , top, offsetX, offsetY, 180, 90);
				path.CloseFigure();

				graphics.DrawPath(&penBorder, &path);
			}
		}
	}
	
	void OnMouseLeave()
	{
		m_dwCurItem = 0;
	}
	
	void OnWindowPosChanged(LPWINDOWPOS lpWndPos)
	{
		m_rcWindow.left = lpWndPos->x;
		m_rcWindow.top = lpWndPos->y;
		m_rcWindow.right = lpWndPos->x + lpWndPos->cx;
		m_rcWindow.bottom = lpWndPos->y + lpWndPos->cy;


		m_rcBar = m_rcWindow;
		m_rcValidte = m_rcBar;
		if(m_bHorizontalBar)
		{
			m_rcBar.bottom = m_rcWindow.bottom -2;
			m_rcBar.top = m_rcWindow.top + 2;
			m_rcValidte.top -=2;
			m_rcValidte.bottom += 2;
		}
		else
		{
			m_rcBar.right = m_rcWindow.right - 2;
			m_rcBar.left = m_rcWindow.left + 2;
			m_rcValidte.left -= 2;
			m_rcValidte.right += 2;
		}

		m_rcBg = m_rcValidte;
	}

	LRESULT OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam)
	{
		LPSIZE pSize = (LPSIZE)lParam;

		pSize->cx = m_nBarWidth;
		pSize->cy = m_nBarHeight;

		return S_OK;
	}

	void OnLButtonDown(UINT nFlags, CPoint point)
	{
		if(m_bHorizontalBar)
		{	
			OnLButtonDownHorizontal(nFlags, point);
		}
		else
		{
			OnLButtonDownVertical(nFlags, point);
		}
	}

	void OnLButtonDownHorizontal(UINT nFlags, CPoint& point)
	{
		int nOffset = 0;
		SetMsgHandled(TRUE);
		CRect rcBar(m_rcBar);

		rcBar.top -= 2;
		rcBar.bottom += 2;

		if (rcBar.PtInRect(point))
		{
			m_dwCurItem = 1;

			m_lastPoint = point;
		}
		else if (m_rcBg.PtInRect(point))
		{
			m_dwCurItem = 2;

			if (point.x > m_rcBar.right)
			{
				nOffset = point.x - m_rcBar.right;
			}
			else if (point.x < m_rcBar.left)
			{
				nOffset = point.x - m_rcBar.left;
			}
		}

		if (nOffset != 0)
		{
			ScrollOffset(nOffset);
		}

		CBkWindow* pWndParent = BkWnds::GetWindow(m_hBkWndParent);
		if (pWndParent)
		{
			pWndParent->BkSendMessage(WM_BK_SCROLL_LBUTTON, WM_LBUTTONDOWN, 0);
		}
	}

	void OnLButtonDownVertical(UINT nFlags, CPoint& point)
	{
		int nOffset = 0;
		SetMsgHandled(TRUE);
		CRect rcBar(m_rcBar);

		rcBar.left -= 2;
		rcBar.right += 2;

		if (rcBar.PtInRect(point))
		{
			m_dwCurItem = 1;

			m_lastPoint = point;
		}
		else if (m_rcBg.PtInRect(point))
		{
			m_dwCurItem = 2;

			if (point.y > m_rcBar.bottom)
			{
				nOffset = point.y - m_rcBar.bottom;
			}
			else if (point.y < m_rcBar.top)
			{
				nOffset = point.y - m_rcBar.top;
			}
		}

		if (nOffset != 0)
		{
			ScrollOffset(nOffset);
		}

        CBkWindow* pWndParent = BkWnds::GetWindow(m_hBkWndParent);
        if (pWndParent)
        {
            pWndParent->BkSendMessage(WM_BK_SCROLL_LBUTTON, WM_LBUTTONDOWN, 0);
        }
	}

	void OnLButtonUp(UINT nFlags, CPoint point)
	{
		m_dwCurItem = 0;

        CBkWindow* pWndParent = BkWnds::GetWindow(m_hBkWndParent);
        if (pWndParent)
        {
            pWndParent->BkSendMessage(WM_BK_SCROLL_LBUTTON, WM_LBUTTONUP, 0);
        }
	}

	void OnMouseMove(UINT nFlags, CPoint point)
	{
		if (m_dwCurItem == 1)
		{
			//处理拖动
			int nOffset = 0;
			if(m_bHorizontalBar)
			{
				nOffset = m_lastPoint.x - point.x;
			}
			else
			{
				nOffset = m_lastPoint.y - point.y;
			}
			
			ScrollOffset(-nOffset);
			m_lastPoint = point;
		}
	}

	 HRESULT OnAttributeColorBarChange(CStringA& strValue, BOOL bLoading)
	 {
		 DWORD dwValue = CBkObject::HexStringToULong(strValue);
		
		 m_color.SetValue(dwValue);

		 return S_OK;
	 }

	 HRESULT OnAttributeColorBarHvChange(CStringA& strValue, BOOL bLoading)
	 {
		 DWORD dwValue = CBkObject::HexStringToULong(strValue);
			
		 m_colorHover.SetValue(dwValue);

		 return S_OK;
	 }

	 HRESULT OnAttributeColorBarBorderChange(CStringA& strValue, BOOL bLoading)
	 {
		 DWORD dwValue = CBkObject::HexStringToULong(strValue);

		 m_colorBorder.SetValue(dwValue);
		 return S_OK;
	 }

protected:

	BKWIN_BEGIN_MSG_MAP()
		MSG_WM_WINDOWPOSCHANGED(OnWindowPosChanged)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_NCCALCSIZE(OnNcCalcSize)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
	BKWIN_END_MSG_MAP()

	BKWIN_DECLARE_ATTRIBUTES_BEGIN()
		BKWIN_CUSTOM_ATTRIBUTE("color_bar", OnAttributeColorBarChange)
		BKWIN_CUSTOM_ATTRIBUTE("color_bar_hv", OnAttributeColorBarHvChange)
		BKWIN_CUSTOM_ATTRIBUTE("color_bar_border", OnAttributeColorBarBorderChange)
        BKWIN_INT_ATTRIBUTE("rectanglebar", m_bRectangleBar,TRUE)
        BKWIN_INT_ATTRIBUTE("barwidth",    m_nBarWidth,TRUE)
		BKWIN_INT_ATTRIBUTE("barheight",    m_nBarHeight,TRUE)
		BKWIN_INT_ATTRIBUTE("permanent_hide", m_bPermanentHide,TRUE)
	BKWIN_DECLARE_ATTRIBUTES_END()

protected:

	int		m_nMaxPos; //最大的滚动位置，由内容长度-显示长度
	int		m_nPos; // 当前滚动位置
	int		m_nViewSize; //显示长度
	int		m_nPicSize; // 内容长度
	int		m_nBarLength;
	int     m_dwCurItem;
    BOOL    m_bRectangleBar;
    int     m_nBarWidth; // 滚动条显示宽度
	int	 m_nBarHeight; //滚动条显示长度
	CPoint  m_lastPoint;

	CRect   m_rcValidte;
	CRect	m_rcBar;
	CRect   m_rcBg;
	CStringA m_strSkinBar;
	CStringA m_strSkinBg;
	BOOL     m_bPermanentHide;
	BOOL	  m_bHorizontalBar;

	Gdiplus::Color	m_color;
	Gdiplus::Color	m_colorHover;
	Gdiplus::Color	m_colorBorder;
};

#pragma warning(default:4244)