//********************************************************************
//author:  hexujun@cmcm.com
//create time: 2020/6/18?19:51
//description: 使用gdiplus绘制线条
//*********************************************************************

#ifndef _H_BKLINEVIEW_H_
#define _H_BKLINEVIEW_H_

#include "bkwnd.h"

class CBkLineViewEx : public CBkWindow 
{
public:
	BKOBJ_DECLARE_CLASS_NAME(CBkLineViewEx, "lineviewex")

public:

	CBkLineViewEx() 
	{
		m_nEndCap =  Gdiplus::LineCapFlat;
		m_nStartCap = Gdiplus::LineCapFlat;
		m_nDashStyle = Gdiplus::DashStyleSolid;
		m_colorLine = 0xFF000000;
		m_colorLinePush = m_colorLine;
		m_colorLineHover = m_colorLine;
		m_bVerticalLine = false;
		m_bSetColorHover = m_bSetColorPush = false;
	}


	virtual BOOL NeedRedrawWhenStateChange()
	{
		if (__super::NeedRedrawWhenStateChange()) 
		{
			return TRUE;
		}

		if (m_colorLine != m_colorLinePush || m_colorLine != m_colorLineHover) 
		{
			return FALSE;
		}

		return m_bSetColorHover && m_bSetColorPush;
	}

protected:

	BKWIN_DECLARE_ATTRIBUTES_BEGIN()
		BKWIN_INT_ATTRIBUTE("dashstyle", m_nDashStyle, FALSE)
		BKWIN_CUSTOM_ATTRIBUTE("linecolor", OnAttributeColorLineChange)
		BKWIN_INT_ATTRIBUTE("vertical", m_bVerticalLine, FALSE)
		BKWIN_INT_ATTRIBUTE("startcap", m_nStartCap, FALSE)
		BKWIN_INT_ATTRIBUTE("endcap", m_nEndCap, FALSE)
		BKWIN_CUSTOM_ATTRIBUTE("hover_linecolor", OnAttributeColorHoverChange)
		BKWIN_CUSTOM_ATTRIBUTE("push_linecolor", OnAttributeColorPushChange)
	BKWIN_DECLARE_ATTRIBUTES_END()


	BKWIN_BEGIN_MSG_MAP()
		MSG_WM_PAINT(OnPaint)
	BKWIN_END_MSG_MAP()

protected:

	HRESULT OnAttributeColorLineChange(CStringA& strValue, BOOL bLoading) 
	{
		m_colorLine = HexStringToULong(strValue);
		if(strValue.GetLength() < 8) 
		{
			m_colorLine |= 0xFF000000;
		}

		return S_OK;
	}

	HRESULT OnAttributeColorHoverChange(CStringA& strValue, BOOL bLoading) 
	{
		m_bSetColorHover = true;
		m_colorLineHover = HexStringToULong(strValue);
		if(strValue.GetLength() < 8) 
		{
			m_colorLineHover |= 0xFF000000;
		}

		return S_OK;
	}

	HRESULT OnAttributeColorPushChange(CStringA& strValue, BOOL bLoading) 
	{
		m_bSetColorPush = true;
		m_colorLinePush = HexStringToULong(strValue);
		if(strValue.GetLength() < 8) 
		{
			m_colorLinePush |= 0xFF000000;
		}

		return S_OK;
	}

protected:

	void OnPaint(CDCHandle dc) 
	{
		if(m_rcWindow.IsRectEmpty()) 
		{
			return;
		}

		Gdiplus::ARGB argb = m_colorLine;
		if (m_bSetColorPush && (GetState() & BkWndState_PushDown) == BkWndState_PushDown) 
		{
			argb = m_colorLinePush;
		}
		else if (m_bSetColorHover && (GetState() & BkWndState_Hover) == BkWndState_Hover) 
		{
			argb = m_colorLineHover;
		}

		int nLineWidth = 0;
		Gdiplus::PointF point1;
		Gdiplus::PointF point2;

		if (m_bVerticalLine) 
		{
			nLineWidth = m_rcWindow.Width();
			point1.X = m_rcWindow.left + nLineWidth / 2;
			point1.Y = m_rcWindow.top;
			point2.X = point1.X;
			point2.Y = m_rcWindow.bottom  - 1;
		}
		else 
		{
			nLineWidth = m_rcWindow.Height();
			point1.X = m_rcWindow.left;
			point1.Y = m_rcWindow.top + nLineWidth / 2;
			point2.X = m_rcWindow.right - 1;
			point2.Y = point1.Y;
		}

		Gdiplus::Graphics graphics(dc);
		Gdiplus::Pen penBlack(Gdiplus::Color(argb), nLineWidth);

		penBlack.SetStartCap((Gdiplus::LineCap)m_nStartCap);
		penBlack.SetEndCap((Gdiplus::LineCap)m_nEndCap);
		penBlack.SetDashStyle((Gdiplus::DashStyle)m_nDashStyle);	
		//graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		graphics.DrawLine(&penBlack, point1, point2);
	}

private:
	int m_nDashStyle; //Gdiplus::DashStyle
	int m_nStartCap; // Gdiplus::LineCap
	int m_nEndCap;//  Gdiplus::LineCap
	Gdiplus::ARGB m_colorLine;
	Gdiplus::ARGB m_colorLineHover;
	Gdiplus::ARGB m_colorLinePush;
	bool m_bSetColorHover;
	bool m_bSetColorPush;
	bool m_bVerticalLine;
};

#endif
