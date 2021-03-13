/**
 *  @ CopyRight (C) 2020-2021 Kingsoft
 *  @ Version   :   1.0
 *  @ Author    :   <zhupeihong@cmcm.com>
 *  @ DateTime  :   [2020-11-20 20:20]
 *  @ Brief     :   ªÊ÷∆Ω•±‰±≥æ∞
 **/
#pragma once
#include "bkwnd.h"

class LinearGradientCtrl
    : public CBkWindow
{
    BKOBJ_DECLARE_CLASS_NAME(LinearGradientCtrl, "lineargradient")

public:
    LinearGradientCtrl()
    {
        m_nAngle = 0;
        m_nAlpha = 255;
    }

    ~LinearGradientCtrl()
    {
    }
 
protected:
    void OnPaint(CDCHandle dc)
    {
        DrawBackground(dc);
    }

    void DrawBackground(CDCHandle& dc)
    {
        CRect rt(m_rcWindow);
        Gdiplus::Graphics graphics(dc);

         Gdiplus::LinearGradientBrush linGrBrush(Gdiplus::Rect(rt.left, rt.top, rt.Width(), rt.Height()),
             Gdiplus::Color(m_nAlpha,GetBValue(m_clrstart),GetGValue(m_clrstart),GetRValue(m_clrstart)), Gdiplus::Color(m_nAlpha,GetBValue(m_clrstop),GetGValue(m_clrstop),GetRValue(m_clrstop)), (Gdiplus::REAL)(m_nAngle));

         graphics.FillRectangle(&linGrBrush, Gdiplus::Rect(rt.left, rt.top, rt.Width(), rt.Height()));
    } 


protected:
    BKWIN_DECLARE_ATTRIBUTES_BEGIN()
        BKWIN_HEX_ATTRIBUTE("startcolor", m_clrstart, TRUE)
        BKWIN_HEX_ATTRIBUTE("stopcolor", m_clrstop, TRUE)
        BKWIN_INT_ATTRIBUTE("alpha", m_nAlpha, TRUE)
        BKWIN_INT_ATTRIBUTE("angle", m_nAngle, TRUE)
    BKWIN_DECLARE_ATTRIBUTES_END()

    BKWIN_BEGIN_MSG_MAP()
        MSG_WM_PAINT(OnPaint)
    BKWIN_END_MSG_MAP()

private:
    COLORREF            m_clrstart;
    COLORREF            m_clrstop;
    int                 m_nAngle;
    int                 m_nAlpha;
};