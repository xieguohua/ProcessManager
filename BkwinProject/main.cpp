#include "stdafx.h"
#include "KMainDlg.h"

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
    InitRes();

    KMainDlg dlg;

    dlg.DoModal();

    return 0;
}