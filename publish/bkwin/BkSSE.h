/* -----------------------------------------------------------------------
 *  File		:	BkSSE.h
 *  Author	    :	hexujun
 *  Datet   	:	2015/7/22 16:23
 *  Brief		:	
 *
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------*/

#ifndef __BkSSE__h__
#define __BkSSE__h__
//
///////////////////////////////////////////////////////////////////////////

class CBkSSE
{
public:
	static bool IsSupportSSE();
	static bool DoXor(DWORD dwKey, char * pBuff, int nLen);
	static void DoAnd(DWORD dwKey, char * pBuff, int nLen);
	static void MemSetDWord(LPVOID pBuff, DWORD dwVal, int nSize);
	static void MemCopy(LPVOID pDest, LPVOID pSrc, int nSize);
	static void DoGray(LPVOID pBuff, int nSize);
	static void OpenAlpha(LPVOID pBuff, int nSize);
	static void SetColorKey(LPVOID pBuff, int nSize, DWORD dwColor);
	static void DoOr(DWORD dwKey, LPVOID pBuff, int nLen);
	static DWORD RGBToHSL(DWORD dwColor);
	static DWORD HSLToRGB(DWORD dwHsl);
	static void SetHSL(LPVOID pBuff, int nSize, int h, int s, int l);

	static void MirrorX(LPVOID pBuff, int nWidth, int nHeight);
	static void MirrorY(LPVOID pBuff, int nWidth, int nHeight);
	static void AdjustAlpha(DWORD &dwColor);
	static void Stretch(LPVOID pBuff, int nWidth, int nHeight, LPVOID pDest, int cx, int cy);
private:
	static void MemCopySSE(LPVOID pDest, LPVOID pSrc, int nSize);
	static void SetColorKeySSE(LPVOID pBuff, int nSize, DWORD dwColor);
	static void SetColorKeyNormal(LPVOID pBuff, int nSize, DWORD dwColor);
	static bool DoXorNormal(DWORD dwKey, char * pBuff, int nLen);
	static bool DoXorSSE(DWORD dwKey, char * pBuff, int nLen);
	static void DoOrNormal(DWORD dwKey, LPVOID pBuff, int nLen);
	static void DoOrSSE(DWORD dwKey, LPVOID pBuff, int nLen);
	static void DoAndNormal(DWORD dwKey, char * pBuff, int nLen);
	static void DoAndSSE(DWORD dwKey, char * pBuff, int nLen);
	static void MemSetDWordNormal(LPVOID pBuff, DWORD dwVal, int nSize);
	static void MemSetDWordSSE(LPVOID pBuff, DWORD dwVal, int nSize);
	static void DoGrayNormal(LPVOID pBuff, int nSize);
	static void DoGraySSE(LPVOID pBuff, int nSize);
	static void OpenAlphaNormal(LPVOID pBuff, int nSize);
	static void OpenAlphaSSE(LPVOID pBuff, int nSize);
	static void SetHSLNormal(LPVOID pBuff, int nSize, int h, int s, int l);
	static void SetHSLSSE(LPVOID pBuff, int nSize, int h, int s, int l);
	static int m_nSupportSSE;
};

///////////////////////////////////////////////////////////////////////////
//
#endif // __BkSSE__h__
