//////////////////////////////////////////////////////////////////////////
//  Class Name: BKPngPool
// Description: PNG Pool
//     Creator: ZhangXiaoxuan
//     Version: 2010.2.24 - 1.0 - Create
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <GdiPlus.h>
#pragma comment(lib, "gdiplus.lib")

#include <atlcoll.h>

#include "bkresutil.h"

class BkPngPool
{
public:
    BkPngPool()
    {
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
    }
    virtual ~BkPngPool()
    {
        _Clear();

        Gdiplus::GdiplusShutdown(m_gdiplusToken);
    }

    typedef CAtlMap<UINT, Gdiplus::Image *> _TypePngPool;
    static void RemoveItem(UINT uResID)
    {
        _TypePngPool::CPair* pPairRet = _Instance()->m_mapPng.Lookup(uResID);

        if (NULL != pPairRet)
        {
            delete pPairRet->m_value;

            _Instance()->m_mapPng.RemoveKey(uResID);
        }

        // dpi img
        pPairRet = _Instance()->m_mapDpiPng.Lookup(uResID);

        if (NULL != pPairRet)
        {
            delete pPairRet->m_value;

            _Instance()->m_mapDpiPng.RemoveKey(uResID);
        }
    }

    static Gdiplus::Image* Get(UINT uResID)
    {
        _TypePngPool::CPair* pPairRet = _Instance()->m_mapPng.Lookup(uResID);
        Gdiplus::Image* pImg = NULL;

        if (NULL == pPairRet)
        {
            _LoadPNGImageFromResourceID(uResID, pImg);
            if (pImg)
                _Instance()->m_mapPng[uResID] = pImg;
        }
        else
            pImg = pPairRet->m_value;

        return pImg;
    }

    static Gdiplus::Image* GetDpiImg(UINT uResID, double* pfRate = NULL)
    {
        _TypePngPool::CPair* pPairRet = _Instance()->m_mapDpiPng.Lookup(uResID);
        Gdiplus::Image* pImg = NULL;

        if (NULL == pPairRet)
        {
			double fRate = 0;
            _LoadPNGImageFromDpiResourceID(uResID, pImg, &fRate);
			if (pImg) 
			{
                _Instance()->m_mapDpiPng[uResID] = pImg;
				_Instance()->m_mapDpiRate[uResID] = fRate;
				if (pfRate != NULL) 
				{
					*pfRate = fRate;
				}
			}
        }
		else 
		{
            pImg = pPairRet->m_value;
			CAtlMap<UINT, double>::CPair* pPairRate = _Instance()->m_mapDpiRate.Lookup(uResID);
			if(pfRate != NULL && pPairRate != NULL) 
			{
				*pfRate = pPairRate->m_value;
			}
		}

        return pImg;
    }

    static void Reload()
    {
        _Instance()->_Clear();
    }

    static size_t GetCount()
    {
        return _Instance()->m_mapPng.GetCount();
    }

protected:

    ULONG_PTR m_gdiplusToken;

    _TypePngPool m_mapPng;

    _TypePngPool m_mapDpiPng;

	CAtlMap<UINT, double> m_mapDpiRate;

    static BkPngPool* ms_pInstance;

    static BkPngPool* _Instance()
    {
        if (!ms_pInstance)
            ms_pInstance = new BkPngPool;
        return ms_pInstance;
    }

    static BOOL _LoadPNGImageFromResourceID(UINT nID, Gdiplus::Image* &pImg)
    {
        CStringA strResource;

        BOOL bRet = BkResManager::LoadResource(nID, strResource, BKRES_PNG_TYPE, TRUE);
        int len = strResource.GetLength();

        HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, len);
		if (hMem == NULL)
		{
			return FALSE;
		}
        BYTE* pMem = (BYTE*)::GlobalLock(hMem);
		if (pMem == NULL)
		{
			::GlobalFree(hMem);
			return FALSE;
		}

        memcpy(pMem, (LPCSTR)strResource, len);

        IStream* pStm = NULL;
        ::CreateStreamOnHGlobal(hMem, FALSE, &pStm);
		if (pStm == NULL)
		{
			::GlobalUnlock(hMem);
			::GlobalFree(hMem);
			return FALSE;
		}

        pImg = Gdiplus::Image::FromStream(pStm);

        ::GlobalUnlock(hMem);
        pStm->Release();

        ::GlobalFree(hMem);

        return TRUE;
    }

    static BOOL _LoadPNGImageFromDpiResourceID(UINT nID, Gdiplus::Image* &pImg, double* pFRate = NULL)
    {
        CStringA strResource;

        if (!BkResManager::LoadDpiResource(nID, strResource, BKRES_PNG_TYPE, pFRate))
            return FALSE;

        int len = strResource.GetLength();

        HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, len);
        if (hMem == NULL)
        {
            return FALSE;
        }
        BYTE* pMem = (BYTE*)::GlobalLock(hMem);
        if (pMem == NULL)
        {
            ::GlobalFree(hMem);
            return FALSE;
        }

        memcpy(pMem, (LPCSTR)strResource, len);

        IStream* pStm = NULL;
        ::CreateStreamOnHGlobal(hMem, FALSE, &pStm);
        if (pStm == NULL)
        {
            ::GlobalUnlock(hMem);
            ::GlobalFree(hMem);
            return FALSE;
        }

        pImg = Gdiplus::Image::FromStream(pStm);

        ::GlobalUnlock(hMem);
        pStm->Release();

        ::GlobalFree(hMem);

        return TRUE;
    }

    void _Clear()
    {
        Gdiplus::Image *pImg = NULL;

        POSITION pos = m_mapPng.GetStartPosition();

        while (pos != NULL) 
        {
            pImg = m_mapPng.GetNextValue(pos);
            if (pImg)
                delete pImg;
        }

        m_mapPng.RemoveAll();

        _ClearDpiPng();
    }

    void _ClearDpiPng()
    {
        Gdiplus::Image *pImg = NULL;

        POSITION pos = m_mapDpiPng.GetStartPosition();

        while (pos != NULL) 
        {
            pImg = m_mapDpiPng.GetNextValue(pos);
            if (pImg)
                delete pImg;
        }

        m_mapDpiPng.RemoveAll();
		m_mapDpiRate.RemoveAll();
    }
};

__declspec(selectany) BkPngPool* BkPngPool::ms_pInstance = NULL;