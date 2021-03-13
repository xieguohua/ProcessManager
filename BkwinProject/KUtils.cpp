#include "stdafx.h"
#include "KUtils.h"
#include "framework/KRegister2.h"
#include "framework/KDubaPath.h"

bool KUtils::RestartAsAdmin()
{
    CString strPath = KDubaPath::GetModuleFullPath(NULL);
    return RunAppAsAdmin(strPath, ::GetCommandLine());
}

bool KUtils::RunAppAsAdmin(const CString& strPath, const CString& strCmd)
{
    const int nSuccessCode = 32;
    int nRet = (int) ShellExecute(NULL, L"runas", strPath, strCmd, NULL, SW_SHOW);
    return nRet > nSuccessCode;
}

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
