#include "stdafx.h"
#include "KUtils.h"
#include "framework/KRegister2.h"

void KUtils::SplitString(LPCWSTR src, LPCWSTR token, std::vector<CString>& result)
{
    CString szSrc = src;
    int nTokenPos = 0;
    CString szToken = szSrc.Tokenize(token, nTokenPos);
    while (!szToken.IsEmpty())
    {
        result.push_back(szToken);
        szToken = szSrc.Tokenize(token, nTokenPos);
    }
}

