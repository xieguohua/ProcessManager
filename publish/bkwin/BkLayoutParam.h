/*************************************************
//  File name:       BkLayoutParam.h
//  Date:              2021/01/20
//  <author>      hexujun@cmcm.com
//  <time>          2021/01/20
//   <version>     1.0    
*************************************************/
#ifndef _BkWin_BkLayoutParam_h_
#define _BkWin_BkLayoutParam_h_

#include "IBkWindow.h"
#include "bkobject.h"

namespace BkWin {\

#define BKWIN_BKLAYOUTPARAM_MAX(a, b) (a > b ? a : b)
#define BKWIN_BKLAYOUTPARAM_MIN(a, b) (a > b ? b : a)

#define BKWIN_BKLAYOUTPARAM_DP_ATTRIBUTE(attribname, varname, ChangedFunc) \
	if (attribname == strAttribName) \
	{\
		const int nValue  = m_ownerBkWnd.ScaleByDpi(::StrToIntA(strValue)); \
		const BOOL bChanged = (varname != nValue ? TRUE : FALSE);\
		varname = nValue;\
		ChangedFunc(strAttribName, bLoading, bChanged);\
		hRet = S_OK; \
	} else 
	

#define BKWIN_BKLAYOUTPARAM_INT_ATTRIBUTE(attribname, varname, ChangedFunc) \
	if (attribname == strAttribName) \
	{\
		const int nValue  = ::StrToIntA(strValue); \
		const BOOL bChanged = (varname != nValue ? TRUE : FALSE);\
		varname = nValue;\
		ChangedFunc(strAttribName, bLoading, bChanged);\
		hRet = S_OK; \
	} else 

#define BKWIN_BKLAYOUTPARAM_CUSTOM_ATTRIBUTE(attribname, ChangedFunc) \
	if (attribname == strAttribName) \
	{\
		ChangedFunc(strAttribName, strValue, bLoading);\
		hRet = S_OK; \
	} else 

enum {
	WRAP_CONTENT = -1,
	MATCH_PARENT =-2,
};

class BkLayoutParam : public CBkObject
{
public:

	BkLayoutParam(IBkWindow& ownerBkWnd) : m_ownerBkWnd(ownerBkWnd)
	{
		m_bLayoutPlaceHolder = FALSE;
		m_bLayoutFloat = FALSE;
		m_nLayoutMarginLeft = m_nLayoutMarginRigth = m_nLayoutMarignTop = m_nLayoutMarignBottom = 0;
		m_nLayoutMinHeight = m_nLayoutMinWidth = 0;
		m_nLayoutMaxHeight = m_nLayoutMaxWidth = 0x7FFFFFFF;//最大int值
		m_nLayoutWidth = WRAP_CONTENT;
		m_nLayoutHeight = WRAP_CONTENT;
	}

	~BkLayoutParam()
	{
	}

	bool IsLayoutFloat() const {
		return m_bLayoutFloat == TRUE;
	}

	bool IsLayoutPlaceHolder() const {
		return m_bLayoutPlaceHolder == TRUE;
	}

	int GetLayoutMaringLeft() const {
		return m_nLayoutMarginLeft;
	}

	int GetLayoutMaringRight() const {
		return m_nLayoutMarginRigth;
	}

	int GetLayoutMaringTop() const {
		return m_nLayoutMarignTop;
	}

	int GetLayoutMaringBottom() const {
		return m_nLayoutMarignBottom;
	}

	int GetLayoutMaxWidth() const {
		return m_nLayoutMaxWidth;
	}

	int GetLayoutMaxHeight() const {
		return m_nLayoutMaxHeight;
	}

	int GetLayoutMinWidth() const {
		return m_nLayoutMinWidth;
	}

	int GetLayoutMinHeight() const {
		return m_nLayoutMinHeight;
	}

	int GetLayoutWidth() const {
		return m_nLayoutWidth;
	}

	int GetLayoutHeight() const {
		return m_nLayoutHeight;
	}

	BKWIN_DECLARE_ATTRIBUTES_BEGIN()
		BKWIN_BKLAYOUTPARAM_DP_ATTRIBUTE("layout_margin_left", m_nLayoutMarginLeft, OnUpdateLayoutAttributeChanged)
		BKWIN_BKLAYOUTPARAM_DP_ATTRIBUTE("layout_margin_right", m_nLayoutMarginRigth, OnUpdateLayoutAttributeChanged)
		BKWIN_BKLAYOUTPARAM_DP_ATTRIBUTE("layout_margin_top", m_nLayoutMarignTop, OnUpdateLayoutAttributeChanged)
		BKWIN_BKLAYOUTPARAM_DP_ATTRIBUTE("layout_margin_bottom", m_nLayoutMarignBottom, OnUpdateLayoutAttributeChanged)
		BKWIN_BKLAYOUTPARAM_DP_ATTRIBUTE("layout_place_holder", m_bLayoutPlaceHolder, OnUpdateLayoutAttributeChanged)
		BKWIN_BKLAYOUTPARAM_INT_ATTRIBUTE("layout_float", m_bLayoutFloat, OnUpdateLayoutAttributeChanged)
		BKWIN_BKLAYOUTPARAM_DP_ATTRIBUTE("layout_min_width", m_nLayoutMinWidth, OnUpdateLayoutAttributeChanged)
		BKWIN_BKLAYOUTPARAM_DP_ATTRIBUTE("layout_max_width", m_nLayoutMaxWidth, OnUpdateLayoutAttributeChanged)
		BKWIN_BKLAYOUTPARAM_DP_ATTRIBUTE("layout_min_height", m_nLayoutMinHeight, OnUpdateLayoutAttributeChanged)
		BKWIN_BKLAYOUTPARAM_DP_ATTRIBUTE("layout_max_height", m_nLayoutMaxHeight, OnUpdateLayoutAttributeChanged)
		BKWIN_BKLAYOUTPARAM_CUSTOM_ATTRIBUTE("layout_width", OnUpdateLayoutWidthOrHeightAttributeChanged)
		BKWIN_BKLAYOUTPARAM_CUSTOM_ATTRIBUTE("layout_height", OnUpdateLayoutWidthOrHeightAttributeChanged)
	BKWIN_DECLARE_ATTRIBUTES_END()

protected:
	virtual BOOL IsClass(LPCSTR lpszName) {
		return lpszName && strcmp(GetObjectClass(), lpszName) == 0;
	}

	virtual LPCSTR GetObjectClass() {
		return "BkLayoutParam";
	}

protected:
	void OnUpdateLayoutAttributeChanged(const CStringA& strAttribName, BOOL bLoading, BOOL bChanged) {
		if (bChanged && !bLoading) {
			m_ownerBkWnd.InvalidatePosition();
		}
	}

	void OnUpdateLayoutWidthOrHeightAttributeChanged(const CStringA& strAttribName, const CStringA& strValue, BOOL bLoading) {
		const int nValue = atoi(strValue);

		bool bChanged = false;
		if(strAttribName.Compare("layout_width") == 0) {
			if (nValue == WRAP_CONTENT || nValue == MATCH_PARENT) 
			{
				bChanged = (nValue != m_nLayoutWidth);
				m_nLayoutWidth = nValue;
			}
			else {
				const int nValueDpi = BKWIN_BKLAYOUTPARAM_MAX(0, m_ownerBkWnd.ScaleByDpi(nValue));
				bChanged = (nValueDpi != m_nLayoutWidth);
				m_nLayoutWidth = nValueDpi;
			}
		}
		else if(strAttribName.Compare("layout_height") == 0) {
			if (nValue == WRAP_CONTENT || nValue == MATCH_PARENT) 
			{
				bChanged = (nValue != m_nLayoutHeight);
				m_nLayoutHeight = nValue;
			}
			else {
				int nValueDpi = BKWIN_BKLAYOUTPARAM_MAX(0, m_ownerBkWnd.ScaleByDpi(nValue));
				bChanged = (nValueDpi != m_nLayoutHeight);
				m_nLayoutHeight = nValueDpi;
			}
		}

		if(bChanged && bLoading) {
			OnUpdateLayoutAttributeChanged(strAttribName, bLoading, bChanged);
		}
	}

protected:
	IBkWindow& m_ownerBkWnd;

protected:

	/**
	 * 针对linearlayout：控件左边距空白区域
	 */
	int m_nLayoutMarginLeft;

	/**
	 * 针对linearlayout：控件右边距空白区域
	 */
	int m_nLayoutMarginRigth;

	/**
	 *  控件上边距空白区域
	 */
	int m_nLayoutMarignTop;

	/**
	 *  控件下边距空白区域
	 */
	int m_nLayoutMarignBottom; 

	int m_nLayoutWidth;//当控件非4个坐标点时，该字段才有效：=WRAP_CONTENT，MATCH_PARENT有特殊含义
	int m_nLayoutHeight;//当控件非4个坐标点时，该字段才有效=WRAP_CONTENT，MATCH_PARENT有特殊含义

	int m_nLayoutMinWidth;//当控件非4个坐标点时，该字段才有效
	int m_nLayoutMaxWidth;//当控件非4个坐标点时，该字段才有效
	int m_nLayoutMinHeight;//当控件非4个坐标点时，该字段才有效
	int m_nLayoutMaxHeight;//当控件非4个坐标点时，该字段才有效

	/**
	 *  对linearlayout, 这个属性=true，控件是隐藏的，表示控件依然展位
	*  这个属性=false，控件是隐藏的，表示控件不站位
	* 默认位置false
	 */
	BOOL m_bLayoutPlaceHolder;

	/**
	 * 针对inearlayout, 这个属性位true，表示该控件不受linearlayout控制
	 */
	BOOL m_bLayoutFloat;
};

}

#endif