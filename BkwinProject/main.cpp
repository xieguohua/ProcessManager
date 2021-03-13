#include "stdafx.h"
#include "KMainDlg.h"
#include <shlobj.h>
#include "KUtils.h"

void InitRes()
{
    #define IDR_DEF_COMMON_SKIN      60000
    #define IDR_DEF_COMMON_STYLE     60001
    #define IDR_DEF_COMMON_STRING    60002

    BkPngPool::Reload();
    BkString::Load(IDR_DEF_COMMON_STRING);
    BkSkin::LoadSkins(IDR_DEF_COMMON_SKIN);
    BkStyle::LoadStyles(IDR_DEF_COMMON_STYLE);
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE /*hPrevInstance*/,
                       LPTSTR lpCmdLine,
                       int nCmdShow)
{
    if (!IsUserAnAdmin())
    {
        KUtils::RestartAsAdmin();
        return -1;
    }

    InitRes();

    KMainDlg dlg;

    dlg.DoModal();

    return 0;
}