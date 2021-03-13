#pragma once
class KUtils
{
public:
    static bool RestartAsAdmin();
    static bool RunAppAsAdmin(const CString& strPath, const CString& strCmd);
    static void SplitString(LPCWSTR lpSrc, LPCWSTR lpToken, std::vector<CString>& result);
};