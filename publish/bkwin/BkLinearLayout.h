/*************************************************
//  File name:       BkLinearLayout.h
//  Date:              2021/01/20
//  <author>      hexujun@cmcm.com
//  <time>          2021/01/20
//   <version>     1.0    
// 线性布局
*************************************************/
#ifndef _BkWin_BkLinearLayout_h_
#define _BkWin_BkLinearLayout_h_

#include "bkwnddlg.h"

namespace BkWin {\

class BkLinearLayout : public CBkDialog 
{
public:
	BKOBJ_DECLARE_CLASS_NAME(BkLinearLayout, "linearlayout")

public:

	BkLinearLayout() 
	{
		m_nLayoutReverse = 0;
		m_nLayoutOrientation = 0;
		m_nLayoutSpace = 0;
		m_bClipRegion = TRUE;
	}

	~BkLinearLayout() {

	}

public:
	virtual void RepositionChilds() {
		NotifyInvalidate();
        ReposImpl();
	}

	virtual void RepositionChild(CBkWindow *pBkWndChild) {
		if(!pBkWndChild) {
			return;
		}
        
        ReposImpl();
	}

	virtual BOOL RedrawRegion(CDCHandle& dc, CRgn& rgn) {
		if(m_rcWindow.IsRectEmpty() ) {
			return FALSE;
		}

		if(!rgn.IsNull() && !rgn.RectInRegion(m_rcWindow)) {
			return FALSE;
		}

		if(!m_bClipRegion) {
			return __super::RedrawRegion(dc, rgn);
		}

		CRgn rcRgnClip;
		CRgn rcRgnClipSet;

		rcRgnClipSet.CreateRectRgn(m_rcWindow.left, m_rcWindow.top, m_rcWindow.right, m_rcWindow.bottom);
		if(rgn) {
			rcRgnClipSet.CombineRgn(rgn, RGN_AND);
		}

		rcRgnClip.CreateRectRgn(0, 0, 1, 1);
		rcRgnClip.CopyRgn(rcRgnClipSet);

		int nSaveDC = dc.SaveDC();

		CPoint ptViewPortOrg(0, 0);
		dc.GetViewportOrg(&ptViewPortOrg);
		if (ptViewPortOrg.x || ptViewPortOrg.y)
		{
			rcRgnClipSet.OffsetRgn(ptViewPortOrg.x, ptViewPortOrg.y);
		}
		dc.SelectClipRgn(rcRgnClipSet, RGN_AND);
		
		BOOL bResult = __super::RedrawRegion(dc, rcRgnClip);
		dc.RestoreDC(nSaveDC);

		return bResult;
	}

public:
	BKWIN_DECLARE_ATTRIBUTES_BEGIN()
		BKWIN_BKLAYOUTPARAM_INT_ATTRIBUTE("layout_orientation", m_nLayoutOrientation, OnUpdateLayoutAttributeChanged)
		BKWIN_BKLAYOUTPARAM_INT_ATTRIBUTE("layout_reverse", m_nLayoutReverse, OnUpdateLayoutAttributeChanged)
		BKWIN_BKLAYOUTPARAM_INT_ATTRIBUTE("layout_space", m_nLayoutSpace, OnUpdateLayoutAttributeChanged)
		BKWIN_INT_ATTRIBUTE("clip_region", m_bClipRegion, FALSE)
	BKWIN_DECLARE_ATTRIBUTES_END()

protected:

	void OnUpdateLayoutAttributeChanged(const CStringA& strAttribName, BOOL bLoading, BOOL bChanged) {
		if (bChanged && !bLoading) {
			RepositionChilds();
		}
	}

	virtual void OnChildAttributeChanged(CBkWindow* pChild, LPCSTR lpszAtrrName) {
		if(!pChild) {
			return;
		}
		RepositionChilds();
	}

	void ReportChilds_Horizontal() {

		CBkWindow *pPreLayoutChildWnd = NULL;

		const bool bLeftToRight = m_nLayoutReverse == 0;
		POSITION pos = bLeftToRight ? m_lstWndChild.GetHeadPosition() : m_lstWndChild.GetTailPosition();
		while (pos != NULL)
		{
			CBkWindow* pBkWndChild = bLeftToRight ? m_lstWndChild.GetNext(pos) : m_lstWndChild.GetPrev(pos);
			if (!pBkWndChild) {
				continue;
			}

			if (pBkWndChild->GetLayoutParam().IsLayoutFloat()) {
				// 表示不受layoutview 控制，走绝对布局
				__super::RepositionChild(pBkWndChild);
				continue;
			}


			if(!pBkWndChild->IsVisible() && !pBkWndChild->GetLayoutParam().IsLayoutPlaceHolder()) {
				// 隐藏了，不占坑位，就不参与布局运算
				continue;
			}

			CSize sizeWnd = m_rcWindow.Size();
			 pBkWndChild->BkSendMessage(WM_NCCALCSIZE, TRUE, (LPARAM)&sizeWnd);

			 WINDOWPOS wndPos = {0};
			 wndPos.flags = SWP_NOREPOSITION;
			 wndPos.cx = sizeWnd.cx;
			 wndPos.cy = sizeWnd.cy;
			 wndPos.y = m_rcWindow.top + pBkWndChild->GetLayoutParam().GetLayoutMaringTop();

			 if(bLeftToRight) {
				 if(pPreLayoutChildWnd) {
					 CRect rcPreChild;
					 pPreLayoutChildWnd->GetRect(rcPreChild);
					 wndPos.x = rcPreChild.right + pPreLayoutChildWnd->GetLayoutParam().GetLayoutMaringRight()
						 + m_nLayoutSpace;
				 }
				 else {
					  wndPos.x  = m_rcWindow.left;
				 }

				  wndPos.x += pBkWndChild->GetLayoutParam().GetLayoutMaringLeft();
			 }
			 else {
				 if(pPreLayoutChildWnd) {
					 CRect rcPreChild;
					 pPreLayoutChildWnd->GetRect(rcPreChild);
					 wndPos.x = rcPreChild.left - pPreLayoutChildWnd->GetLayoutParam().GetLayoutMaringLeft() - m_nLayoutSpace;
				 }
				 else {
					 wndPos.x = m_rcWindow.right;
				 }
				 wndPos.x -= pBkWndChild->GetLayoutParam().GetLayoutMaringRight() + wndPos.cx;
			 }

			 pBkWndChild->BkSendMessage(WM_WINDOWPOSCHANGED, NULL, (LPARAM)&wndPos);
			 pPreLayoutChildWnd = pBkWndChild;
		}
	}

	void ReportChilds_Vertical() {

		CBkWindow *pPreLayoutChildWnd = NULL;

		const bool bTopToBottom = m_nLayoutReverse == 0;
		POSITION pos = bTopToBottom ? m_lstWndChild.GetHeadPosition() : m_lstWndChild.GetTailPosition();
		while (pos != NULL)
		{
			CBkWindow* pBkWndChild = bTopToBottom ? m_lstWndChild.GetNext(pos) : m_lstWndChild.GetPrev(pos);
			if (!pBkWndChild) {
				continue;
			}

			if (pBkWndChild->GetLayoutParam().IsLayoutFloat()) {
				// 表示不受layoutview 控制，走绝对布局
				__super::RepositionChild(pBkWndChild);
				continue;
			}


			if(!pBkWndChild->IsVisible() && !pBkWndChild->GetLayoutParam().IsLayoutPlaceHolder()) {
				// 隐藏了，不占坑位，就不参与布局运算
				continue;
			}

			CSize sizeWnd = m_rcWindow.Size();
			pBkWndChild->BkSendMessage(WM_NCCALCSIZE, TRUE, (LPARAM)&sizeWnd);

			WINDOWPOS wndPos = {0};
			wndPos.flags = SWP_NOREPOSITION;
			wndPos.cx = sizeWnd.cx;
			wndPos.cy = sizeWnd.cy;
			wndPos.x = m_rcWindow.left + pBkWndChild->GetLayoutParam().GetLayoutMaringLeft();

			if(bTopToBottom) {
				if(pPreLayoutChildWnd) {
					CRect rcPreChild;
					pPreLayoutChildWnd->GetRect(rcPreChild);
					wndPos.y = rcPreChild.bottom + pPreLayoutChildWnd->GetLayoutParam().GetLayoutMaringBottom() + m_nLayoutSpace;
				}
				else {
					wndPos.y  = m_rcWindow.top;
				}

				wndPos.y += pBkWndChild->GetLayoutParam().GetLayoutMaringTop();
			}
			else {
				if(pPreLayoutChildWnd) {
					CRect rcPreChild;
					pPreLayoutChildWnd->GetRect(rcPreChild);
					wndPos.y = rcPreChild.top - pPreLayoutChildWnd->GetLayoutParam().GetLayoutMaringTop() - m_nLayoutSpace;
				}
				else {
					wndPos.y = m_rcWindow.bottom;
				}
				wndPos.x -= pBkWndChild->GetLayoutParam().GetLayoutMaringBottom() + wndPos.cx;
			}

			pBkWndChild->BkSendMessage(WM_WINDOWPOSCHANGED, NULL, (LPARAM)&wndPos);
			pPreLayoutChildWnd = pBkWndChild;
		}
	}

    void ReposImpl() {
        if(m_nLayoutOrientation == 0) {
            ReportChilds_Horizontal();
        }
        else if(m_nLayoutOrientation == 1)
        {
            ReportChilds_Vertical();
        }
    }

protected:

	BOOL m_bClipRegion;
	int m_nLayoutOrientation;// 方向: =0横向，=1竖直

	/**
	 * 是否反向排列：
	 * m_nLayoutOrientation=0&&m_nLayoutReverse=1，表示right 2 left 排列
	 * m_nLayoutOrientation=1&&m_nLayoutReverse=1，表示从bottom 2 top 排列
	 */
	int m_nLayoutReverse;

	int m_nLayoutSpace;// 布局子元素的间距
};

}
#endif