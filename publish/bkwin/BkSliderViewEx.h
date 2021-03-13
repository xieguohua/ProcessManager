//********************************************************************
//author:  hexujun@cmcm.com
//create time: 2020/6/22?21:45
//description: 描述
//*********************************************************************

#ifndef _H_BKSLIDERVIEWEX_H_
#define _H_BKSLIDERVIEWEX_H_

#include "bkownereventdlg.h"

class CBkSliderViewEx 
	: public CBkOwnerEventDlg 
	, public CBkOwnerEventDlg::IChildCtrlEventListener
{
public:

	enum EmValueChangeFrom 
	{
		EmValueChangeFrom_SelfSet = 0,
		EmValueChangeFrom_ClickBg = 1,
		EmValueChangeFrom_SubBtn = 2,
		EmValueChangeFrom_AddBtn = 3,
		EmValueChangeFrom_DragBar = 4,
		EmValueChangeFrom_MouseWheel = 5,
	};
	class IBkSliderViewExEvent 
	{
	public:
		
		virtual void OnSliderValueChanged(CBkSliderViewEx* pObjView, int nValue, EmValueChangeFrom emFrom)
		{
			OnSliderValueChanged(pObjView, nValue);
		}

		virtual void OnSliderValueChanged(CBkSliderViewEx* pObjView, int nValue){}
		virtual void OnStartDragSliderBar(CBkSliderViewEx* pObjView){}
		virtual void OnEndDragSliderBar(CBkSliderViewEx* pObjView){}
	};

	BKOBJ_DECLARE_CLASS_NAME(CBkSliderViewEx, "sliderviewex")

	enum 
	{
		IDC_BTN_SUB = 1,
		IDC_BTN_Add = 2,
		IDC_BG = 3,
		IDC_VALUE = 4,
		IDC_BTN_SLIDE_BAR = 5,
	};
	
	CBkSliderViewEx() 
	{
		m_bDragSlideBar = false;
		m_nMaxValue = 100;
		m_nCurrentValue = 0;
		m_nStepValue = 1;
		m_pBtnSlider = NULL;
		m_pWndValue = NULL;
		m_pSliderListener = NULL;
		m_bEnableMouseWhell = TRUE;
		m_ptLast.SetPoint(0, 0);
		SetListener(this);
	}

	~CBkSliderViewEx() 
	{
	}

	int GetMaxVaue() const
	{
		return m_nMaxValue;
	}

	int GetCurrentValue() const 
	{
		return m_nCurrentValue;
	}

	void SetMaxValue(int nMaxValue) 
	{
		SetMaxValueImp(nMaxValue, true);
	}
	
	void SetCurrentValue(int nValue) 
	{
		SetCurrentValueImpl(nValue, true, EmValueChangeFrom_SelfSet);
	}

	void SetEnableStatus(BOOL bValue, 
		LPCSTR lpszSliderValueClass = "slider_value", 
		LPCSTR lpszSliderValueGreyClass = "slider_value_grey",
		LPCSTR lpszSliderBarClass = "slider_bar",
		LPCSTR lpszSliderBarGreyClass = "slider_bar_grey")
	{
		//约定：slider_value 为enable状态，slider_value_grey为disable的状态
		this->SetAttribute("transparent", bValue ? "0": "1", FALSE);
		this->FindChildByCmdID(IDC_BTN_SLIDE_BAR)->SetAttribute("class", bValue ? lpszSliderBarClass : lpszSliderBarGreyClass, FALSE);
		this->FindChildByCmdID(IDC_VALUE)->SetAttribute("class", bValue ? lpszSliderValueClass : lpszSliderValueGreyClass, FALSE);

		NotifyInvalidate();
	}

	void SetSliderBarListener(IBkSliderViewExEvent* pListener) 
	{
		m_pSliderListener = pListener;
	}

	virtual BOOL Load(TiXmlElement* pTiXmlElem) 
	{
		BOOL bSuccess = __super::Load(pTiXmlElem);
		m_pBtnSlider = FindChildByCmdID(IDC_BTN_SLIDE_BAR);
		m_pWndValue = FindChildByCmdID(IDC_VALUE);
		m_pBgWnd = FindChildByCmdID(IDC_BG);

		CBkWindow* pValueWnd = FindChildByCmdID(IDC_VALUE);
		if (pValueWnd != NULL) 
		{
			pValueWnd->SetAttribute("transparent", "1", TRUE);
		}

		return bSuccess;
	}

	virtual void OnChildEventLButtonDown(CBkOwnerEventDlg& ownerDlg, CBkWindow& child, const CPoint& point)
	{
		if(child.GetCmdID() == IDC_BTN_SLIDE_BAR) 
		{
			m_bDragSlideBar = true;
			m_ptLast = point;
			if(m_pSliderListener != NULL) 
			{
				m_pSliderListener->OnStartDragSliderBar(this);
			}
			return;
		}

		if(child.GetCmdID() == IDC_BG) 
		{
			CRect rcWnd;

			child.GetRect(rcWnd);
			double fPercert = (point.x - rcWnd.left) * 1.0 / rcWnd.Width();
			int nValue = m_nMaxValue * fPercert;
			SetCurrentValueImpl(nValue, true, EmValueChangeFrom_ClickBg);
		}
	}

	virtual void OnChildEventLButtonUp(CBkOwnerEventDlg& ownerDlg, CBkWindow& child, const CPoint& point)
	{
		if(child.GetCmdID() == IDC_BTN_Add) 
		{
			SetCurrentValueImpl(GetCurrentValue() + m_nStepValue, true, EmValueChangeFrom_AddBtn);
		}
		else if(child.GetCmdID() == IDC_BTN_SUB) 
		{
			SetCurrentValueImpl(GetCurrentValue() - m_nStepValue, true, EmValueChangeFrom_SubBtn);
		}
		else if (child.GetCmdID() == IDC_BTN_SLIDE_BAR) {

			if (m_bDragSlideBar)
			{
				m_bDragSlideBar = false;
				if(m_pSliderListener != NULL) 
				{
					m_pSliderListener->OnEndDragSliderBar(this);
				}
			}
		}
	}


	virtual void OnChildEventMouseMove(CBkOwnerEventDlg& ownerDlg, CBkWindow& child, const CPoint& point)
	{
		if(child.GetCmdID() != IDC_BTN_SLIDE_BAR) 
		{
			return;
		}

		if (m_pBgWnd == NULL || !m_bDragSlideBar) 
		{
			return;
		}
		
		CRect rcWnd;
		m_pBgWnd->GetRect(rcWnd);
		double fPercert = (point.x - rcWnd.left) * 1.0 / rcWnd.Width();
		int nValue = fPercert * m_nMaxValue;
		SetCurrentValueImpl(nValue, true, EmValueChangeFrom_DragBar);
	}

	virtual void OnChildEventMouseLeave(CBkOwnerEventDlg& ownerDlg, CBkWindow& child) 
	{
		if(child.GetCmdID() == IDC_BTN_SLIDE_BAR && m_bDragSlideBar) 
		{
			m_bDragSlideBar = false;
			if(m_pSliderListener != NULL) 
			{
				m_pSliderListener->OnEndDragSliderBar(this);
			}
		}
	}



public:
	BKWIN_DECLARE_ATTRIBUTES_BEGIN()
		BKWIN_CUSTOM_ATTRIBUTE("current_value", OnCurrentValueAttributeChange, FALSE)
		BKWIN_CUSTOM_ATTRIBUTE("max_value", OnMaxValueAttributeChange, FALSE)
		BKWIN_INT_ATTRIBUTE("step_value", m_nStepValue, FALSE)
		BKWIN_INT_ATTRIBUTE("enable_mousewhell", m_bEnableMouseWhell, FALSE)
	BKWIN_DECLARE_ATTRIBUTES_END()

	BKWIN_BEGIN_MSG_MAP()
		MSG_WM_MOUSEWHEEL(OnMouseWhell)
	BKWIN_END_MSG_MAP()

protected:


	BOOL OnMouseWhell( UINT nFlags, short zDelta, CPoint point )
	{
		if(__super::OnMouseWhell(nFlags, zDelta, point)) 
		{
			return TRUE;
		}

		if(!m_bEnableMouseWhell) 
		{
			return FALSE;
		}

		int nOldValue = GetCurrentValue();
		SetCurrentValueImpl(zDelta < 0 ? GetCurrentValue() + m_nStepValue : GetCurrentValue() - m_nStepValue, true, EmValueChangeFrom_MouseWheel);

		return nOldValue != GetCurrentValue();
	}

	HRESULT OnCurrentValueAttributeChange(CStringA& strValue, BOOL bLoading) 
	{
		SetCurrentValueImpl(atoi(strValue), !bLoading, EmValueChangeFrom_SelfSet);

		return S_OK;
	}

	HRESULT OnMaxValueAttributeChange(CStringA& strValue, BOOL bLoading) 
	{
		SetMaxValueImp(atoi(strValue), !bLoading);
		return S_OK;
	}

protected:

	void SetMaxValueImp(int nMaxValue, bool bLayout) 
	{
		if(nMaxValue < 1) 
		{
			nMaxValue = 1;
		}

		if (nMaxValue == m_nMaxValue) 
		{
			return;
		}

		m_nMaxValue = nMaxValue;
		if(bLayout) 
		{
			RepositionChilds();
			NotifyInvalidate();
		}
	}
	
	void SetCurrentValueImpl(int nValue, bool bLayout, EmValueChangeFrom from) 
	{
		if (nValue < 0) 
		{
			nValue = 0;
		}

		if(nValue > m_nMaxValue)
		{
			nValue = m_nMaxValue;
		}

		if (nValue == m_nCurrentValue) 
		{
			return;
		}

		m_nCurrentValue = nValue;
		if(bLayout)
		{
			RepositionChilds();
			NotifyInvalidate();
		}

		if (m_pSliderListener != NULL) 
		{
			m_pSliderListener->OnSliderValueChanged(this, m_nCurrentValue, from);
		}
	}

	virtual void RepositionChild(CBkWindow *pBkWndChild)
	{
		if(pBkWndChild == NULL) 
		{
			return;
		}

		__super::RepositionChild(pBkWndChild);

		if(pBkWndChild->GetCmdID() != IDC_VALUE &&
			pBkWndChild->GetCmdID() != IDC_BTN_SLIDE_BAR) 
		{
			return;
		}

		CBkWindow* pTarget = m_pBgWnd;
		if (pTarget == NULL) 
		{
			pBkWndChild->SetVisible(FALSE);
			return;
		}

		CRect rcChild;
		pBkWndChild->GetRect(rcChild);

		CRect rcTargetValue;
		pTarget->GetRect(rcTargetValue);

		double fPercent = m_nCurrentValue * 1.0 / m_nMaxValue;
		if(fPercent > 1) 
		{
			fPercent = 1.0;
		}

		if(pBkWndChild->GetCmdID() == IDC_BTN_SLIDE_BAR) 
		{
			int nWidthSlidBar = rcChild.Width();
			rcChild.left = rcTargetValue.left + rcTargetValue.Width() * fPercent - nWidthSlidBar / 2;
			rcChild.right = rcChild.left + nWidthSlidBar;
		}
		else 
		{
			rcChild.right = rcChild.left + rcTargetValue.Width() * fPercent;
		}
		pBkWndChild->SetRect(rcChild);
	}

	void RepostSpecChild(CBkWindow* pBkWndChild, const CRect& rcWindow)
	{
		BKDLG_POSITION dlgPos;
		WINDOWPOS WndPos = {0};
		CRect rcDlg = rcWindow;

		pBkWndChild->GetDlgPosition(&dlgPos);

		if (dlgPos.Left.bCenter)
			WndPos.x = rcDlg.left + rcDlg.Width() / 2 + (dlgPos.Left.bMinus ? -1 : 1) *  dlgPos.Left.nPos;
		else if (dlgPos.Left.bMinus)
			WndPos.x = rcDlg.left + rcDlg.Width() - dlgPos.Left.nPos;
		else
			WndPos.x = rcDlg.left + dlgPos.Left.nPos;

		if (dlgPos.Top.bCenter)
			WndPos.y = rcDlg.top + rcDlg.Height() / 2 + (dlgPos.Top.bMinus ? -1 : 1) *  dlgPos.Top.nPos;
		else if (dlgPos.Top.bMinus)
			WndPos.y = rcDlg.top + rcDlg.Height() - dlgPos.Top.nPos;
		else
			WndPos.y = rcDlg.top + dlgPos.Top.nPos;

		WndPos.cx = rcDlg.Width();
		WndPos.cy = rcDlg.Height();
		pBkWndChild->BkSendMessage(WM_WINDOWPOSCHANGED, NULL, (LPARAM)&WndPos);
	}

protected:
	int m_nMaxValue;
	int m_nCurrentValue;
	int m_nStepValue;
	CPoint m_ptLast;
	bool m_bDragSlideBar;
	CBkWindow* m_pBtnSlider;
	CBkWindow* m_pWndValue;
	CBkWindow* m_pBgWnd;
	IBkSliderViewExEvent* m_pSliderListener;
	BOOL m_bEnableMouseWhell;
};


#endif