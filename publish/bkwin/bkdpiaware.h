/*
*************
* DPI Unaware
* DPI unaware applications render as if the screen that they are on has a DPI value of 96. 
* Whenever these applications are run on a screen with a display scale greater than 100% (> 96 DPI), 
* Windows will stretch the application bitmap to the expected physical size, although this results in the application being blurry

* System DPI Awareness
* Desktop applications that are system DPI aware typically detect the DPI of the primary connected monitor on startup. 
* During initialization, they layout their UI appropriately (sizing controls, choosing font sizes, loading assets, etc.) 
* for that single DPI. System DPI-aware applications are not DPI scaled by Windows (bitmap stretched) on the primary display. 
* When the application is moved to a display with a different scale factor, Windows will bitmap stretch the application bitmap, 
* which can result in it being blurry. Effectively, System-DPI-aware desktop applications only render correctly at a single display 
* scale factor and become blurry whenever the DPI changes

* Per-Monitor and Per-Monitor (V2) DPI Awareness
* It is recommended that desktop applications are updated to use per-monitor DPI awareness mode in order to render correctly whenever the DPI of the display that they re running on changes. 
* When an application reports to Windows that it wants to run in this mode, Windows will step out of the way and not bitmap stretch the application when the DPI changes. 
* It is completely the application s responsibility to handle resizing itself for the new DPI. The reason that work is required here, by the application, 
* is that most UI frameworks that desktop application use (Windows common controls (comctl32), Windows Forms, Windows Presentation Framework, etc.) do not support automatic DPI scaling by default.

*************
* Support system detail
* https://docs.microsoft.com/zh-cn/windows/desktop/hidpi/high-dpi-desktop-application-development-on-windows#system-dpi-awareness
*/

#pragma once
#include <vector>
#include <algorithm>
#include <functional>

#define BKWNDPI_NORMAL                  96
#define DPISETTING_GROUP                _T("basesetting")
#define DPISETTING_SWITCH               _T("switch")
#define DPISETTING_DEBUG                _T("debug")
#define DPISETTING_DEBUGDPI             _T("debugdpi")
#define DPISETTING_SUPPORTRATELIST      _T("supportrate")
#define DPISETTING_MODE                 _T("mode")
#define DPISETTING_SUPPORT_PROCESS      _T("supportprocess")
#define DPIAWARE_SETTING_PATH           _T("\\ressrc\\chs\\dpisetting.ini")
#define DPIRESOURCE_PATH                _T("\\ressrc\\chs\\dpiimg")
#define HIDDEN_DPIAWARE_WND_CLASSNAME    _T("hiddenDpiAwarenessWindow")

class BKWinDpiAware
{
public:
    enum enDpiAwarenessContext
    {
        DPI_AWARENESS_UNAWARE = -1,
        DPI_AWARENESS_SYSTEM_AWARE = -2,
        DPI_AWARENESS_PER_MONITOR_AWARE = -3,
        DPI_AWARENESS_PER_MONITOR_AWARE_V2 = -4,
    };

    enum enDpiMode
    {
        UNKNOWN_MODE = 0,
        PROCESS_CONTEXT_MODE = 1,
        THREAD_CONTEXT_MODE = 2,
    };
    typedef std::vector<double> SupportDpiRateVec;

    ~BKWinDpiAware(){}

    static BKWinDpiAware& GetInstance(){
        static BKWinDpiAware instance;
        return instance;
    }

    DWORD GetCurrentDpi()
    {
        if (is_debug_mode())
            return get_debug_dpi();

        if (m_dwCurDpi == 0)
            m_dwCurDpi = GetDpiByWindowClassName(get_hidden_dpiaware_wnd_name());
        return m_dwCurDpi;
    }

    void ChangeCurrentDpi(DWORD dwNewDpi, DWORD& dwOldDpi)
    {
        dwOldDpi = m_dwCurDpi;
        m_dwCurDpi = dwNewDpi;
    }

    float GetDpiScaleRate()
    {
        double fRate = ((double)GetCurrentDpi()) / BKWNDPI_NORMAL;
        SupportDpiRateVec& vecRate = get_dpi_support_rate_list();
        SupportDpiRateVec::const_reverse_iterator iter = std::find_if(vecRate.rbegin(), vecRate.rend(),
            std::bind2nd(std::less_equal<double>(), fRate));
        return ( ( iter == vecRate.rend() || *iter < 1.0 ) ? 1.0 : *iter );
    }

    DWORD GetDpiByWindowClassName(const CString& strClassName)
    {
        HWND hWnd = ::FindWindow(strClassName, NULL);
        return GetDpiByWindowHandle(hWnd);
    }

    DWORD GetDpiByWindowHandle(HWND hWnd)
    {
        if (!hWnd)
            return BKWNDPI_NORMAL;

        HMODULE hUser32 = LoadLibrary(_T("user32.dll"));
        if (!hUser32)
            return BKWNDPI_NORMAL;

        typedef UINT (WINAPI* funGetDpiForWindow)(HWND);
        funGetDpiForWindow pFun = (funGetDpiForWindow)GetProcAddress(hUser32, "GetDpiForWindow");

        DWORD dwDpi = BKWNDPI_NORMAL;
        if (pFun)
            dwDpi = pFun(hWnd);
        else if (get_dpi_mode() == PROCESS_CONTEXT_MODE)
        {
            HDC hdc = ::GetDC(hWnd);
            dwDpi = ::GetDeviceCaps(hdc, LOGPIXELSX);
            ::ReleaseDC(hWnd, hdc);
        }

        FreeLibrary(hUser32);
        return dwDpi;
    }

    BOOL IsDpiSwitchOn()
    {
        if (m_bSwitch == -1)
            m_bSwitch = (is_dpiaware_local_switch_on() && is_dpiaware_setting_on()) || is_debug_mode();
        return m_bSwitch;
    }

    BOOL IsDpiLocalSwitchOn()
    {
        return is_dpiaware_local_switch_on();
    }

    long ScaleByDpi(long lSize)
    {
        return IsDpiSwitchOn() ? (long)(lSize * GetDpiScaleRate()) : lSize;
    }

	long UnScaleByDpi(long lSize)
	{
		return IsDpiSwitchOn() ? (long)(lSize / GetDpiScaleRate()) : lSize;
	}

    int SetThreadDpiAwarenessContext(enDpiAwarenessContext context)
    {
        if (!IsDpiSwitchOn() || get_dpi_mode() != THREAD_CONTEXT_MODE)
            return 0;

        HMODULE hUser32 = LoadLibrary(_T("user32.dll"));
        if (!hUser32)
            return 0;

        typedef int (WINAPI* funSetThreadDpiAwarenessContext)(int);
        funSetThreadDpiAwarenessContext pFun = (funSetThreadDpiAwarenessContext)GetProcAddress(hUser32, "SetThreadDpiAwarenessContext");

        int oldContext = 0;
        if (pFun)
            oldContext = pFun(context);
        FreeLibrary(hUser32);
        return oldContext;
    }

    BOOL SetProcessDpiAwarenessContext(enDpiAwarenessContext context)
    {
        if (!IsDpiSwitchOn() || get_dpi_mode() != PROCESS_CONTEXT_MODE)
            return 0;

        HMODULE hUser32 = LoadLibrary(_T("user32.dll"));
        if (!hUser32)
            return 0;

        typedef BOOL (WINAPI* funSetThreadDpiAwarenessContext)(int);
        funSetThreadDpiAwarenessContext pFun1 = (funSetThreadDpiAwarenessContext)GetProcAddress(hUser32, "SetProcessDpiAwarenessContext");

        typedef BOOL (WINAPI* LPSetProcessDPIAware)(void);
        LPSetProcessDPIAware pFun2 = (LPSetProcessDPIAware)GetProcAddress(hUser32, "SetProcessDPIAware");

        BOOL bResult = FALSE;
        if (pFun1)
            bResult = pFun1(context);
        else if (pFun2)
            bResult = pFun2();

        FreeLibrary(hUser32);
        return bResult;
    }

    CString GetDpiResourcePath()
    {
        return get_dpi_resource_path();
    }

    BOOL IsSupportOs(BOOL& bSupportNow)
    {
        HMODULE hUser32 = LoadLibrary(_T("user32.dll"));
        if (!hUser32)
            return FALSE;

        BOOL bSupport = FALSE;
        typedef BOOL (WINAPI* funSetThreadDpiAwarenessContext)(int);
        funSetThreadDpiAwarenessContext pFun1 = (funSetThreadDpiAwarenessContext)GetProcAddress(hUser32, "SetProcessDpiAwarenessContext");

        typedef BOOL (WINAPI* LPSetProcessDPIAware)(void);
        LPSetProcessDPIAware pFun2 = (LPSetProcessDPIAware)GetProcAddress(hUser32, "SetProcessDPIAware");

        typedef int (WINAPI* funSetThreadDpiAwarenessContext)(int);
        funSetThreadDpiAwarenessContext pFun = (funSetThreadDpiAwarenessContext)GetProcAddress(hUser32, "SetThreadDpiAwarenessContext");

        bSupport = (pFun != NULL || pFun1 != NULL || pFun2 != NULL);

        if (get_dpi_mode() == PROCESS_CONTEXT_MODE)
            bSupportNow = (pFun1 != NULL || pFun2 != NULL);
        else if (get_dpi_mode() == THREAD_CONTEXT_MODE)
            bSupportNow = (pFun != NULL);

        FreeLibrary(hUser32);
        return bSupport;
    }


	SupportDpiRateVec& get_dpi_support_rate_list()
	{
		if (m_vecSupportRateList.empty())
		{
			CString strSupportRate;
			::GetPrivateProfileString(DPISETTING_GROUP, DPISETTING_SUPPORTRATELIST, _T("1.0|1.2|1.5"), strSupportRate.GetBuffer(MAX_PATH), MAX_PATH, get_dpi_setting_file());
			strSupportRate.ReleaseBuffer();
			for (int iPos = 0; TRUE; )
			{
				CString strRate = strSupportRate.Tokenize(_T("|"), iPos);
				if (strRate.IsEmpty())
					break;
				m_vecSupportRateList.push_back(_wtof(strRate));
			}
		}
		return m_vecSupportRateList;
	}

	enum enum_dpi_aware_setting_on_mem
	{
		enum_dpi_aware_setting_on_mem_null = -1,
		enum_dpi_aware_setting_on_mem_close = 0,
		enum_dpi_aware_setting_on_mem_open = 1,
	};	

	static void set_dpi_aware_setting_on_mem(enum_dpi_aware_setting_on_mem nValue) 
	{
		s_dpiaware_setting_on_mem = nValue;
	}


private:
    BKWinDpiAware()
    : m_bInit(FALSE)
    , m_bDebug(-1)
    , m_iDebugDpi(-1)
    , m_bSwitch(-1)
    , m_dwCurDpi(0)
    , m_iDpiMode(UNKNOWN_MODE)
    {
        init();
    }
    BKWinDpiAware(const BKWinDpiAware& da);
    BKWinDpiAware& operator = (const BKWinDpiAware& da);

    BOOL create_hidden_dpi_awareness_window()
    {
        int oldContext = SetThreadDpiAwarenessContext(DPI_AWARENESS_PER_MONITOR_AWARE_V2);
        HWND hWnd = CreateWindowEx(NULL, get_hidden_dpiaware_wnd_name(), NULL, WS_POPUPWINDOW, CW_USEDEFAULT, 0, 0, 0, ::GetDesktopWindow(), NULL, NULL, NULL);
        SetThreadDpiAwarenessContext((enDpiAwarenessContext)oldContext);
        return hWnd != NULL;
    }

    BOOL is_hidden_dpi_awareness_window_exist()
    {
        return (::FindWindow(get_hidden_dpiaware_wnd_name(), NULL) != NULL);
    }

    BOOL is_dpiaware_setting_on()
    {
		if(s_dpiaware_setting_on_mem != enum_dpi_aware_setting_on_mem_null) {
			return s_dpiaware_setting_on_mem == enum_dpi_aware_setting_on_mem_open;
		}

        BOOL bSupportNow = FALSE;
        DWORD dwDpiAware = FALSE;
        if (IsSupportOs(bSupportNow) && bSupportNow)
        {
#if _BK_DPIAWARE_SETTING_ON_
			return TRUE;
#endif


            CRegKey regKey;
            CString strRegPath;
            strRegPath.Format(L"%s\\%s", _T("Software\\Kingsoft\\Antivirus"), L"KAVReport");
            if (ERROR_SUCCESS == regKey.Create(HKEY_LOCAL_MACHINE, strRegPath))
                regKey.QueryDWORDValue(L"DpiAware", dwDpiAware);
            regKey.Close();
        }
        return (BOOL)dwDpiAware;
    }

    BOOL is_dpiaware_local_switch_on()
    {
        return ::GetPrivateProfileInt(DPISETTING_GROUP, DPISETTING_SWITCH, FALSE, get_dpi_setting_file());
    }

    void register_hidden_dpi_awareness_window()
    {
        CString strClassName = get_hidden_dpiaware_wnd_name();
        WNDCLASSEX wcex;
        memset(&wcex, 0, sizeof(wcex));
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = DefWindowProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = NULL;
        wcex.hIcon = NULL;
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = strClassName;
        wcex.hIconSm = NULL;
        RegisterClassEx(&wcex);
    }

    void init()
    {
        if (InterlockedExchange((long*)&m_bInit, TRUE) == FALSE)
        {
            if (!is_supported_process())
                return;

            if (get_dpi_mode() == PROCESS_CONTEXT_MODE)
                SetProcessDpiAwarenessContext(DPI_AWARENESS_PER_MONITOR_AWARE_V2);

            if (!is_hidden_dpi_awareness_window_exist())
            {
                register_hidden_dpi_awareness_window();
                create_hidden_dpi_awareness_window();
            }
        }
    }

    BOOL is_supported_process()
    {
        CString strSupportProcessList = get_dpi_support_process_list();
        CString strExeName = path_to_file_name(get_module_full_path());
        return strSupportProcessList.Find(strExeName) != -1;
    }

    enDpiMode get_dpi_mode()
    {
        if (m_iDpiMode == UNKNOWN_MODE)
            m_iDpiMode = (enDpiMode)::GetPrivateProfileInt(DPISETTING_GROUP, DPISETTING_MODE, THREAD_CONTEXT_MODE, get_dpi_setting_file());
        return m_iDpiMode;
    }

    CString get_module_path()
    {
        TCHAR szFileName[MAX_PATH + 1] = {0};
        if (!::GetModuleFileName(NULL, szFileName, MAX_PATH))
            return _T("");

        CString strPath = szFileName;
        ::PathRemoveFileSpec(strPath.GetBuffer(MAX_PATH));
        strPath.ReleaseBuffer();
        return strPath;
    }

    CString get_module_full_path()
    {
        TCHAR szFileName[MAX_PATH + 1] = {0};
        if (!::GetModuleFileName(NULL, szFileName, MAX_PATH))
            return _T("");

        CString strPath = szFileName;
        return strPath;
    }

    CString path_to_file_name(LPCTSTR szFullPath)
    {
        CString strPath(szFullPath);

        strPath.Replace(_T('/'), _T('\\'));
        int nPos = strPath.ReverseFind(_T('\\'));
        if (nPos != -1)
            strPath = strPath.Right(strPath.GetLength() - nPos - 1);

        nPos = strPath.ReverseFind('.');
        if (nPos != -1)
            strPath = strPath.Left(nPos);
        return strPath;
    }

    CString get_hidden_dpiaware_wnd_name()
    {
        CString strName;
        strName.Format(_T("%s_%s"), path_to_file_name(get_module_full_path()), HIDDEN_DPIAWARE_WND_CLASSNAME);
        return strName;
    }

    CString get_dpi_setting_file()
    {
        CString strDpiSettingFile = get_module_path();
        if (strDpiSettingFile.IsEmpty())
            return _T("");

        strDpiSettingFile.Append(DPIAWARE_SETTING_PATH);
        return strDpiSettingFile;
    }

    CString get_dpi_resource_path()
    {
        CString strDpiResourceFile = get_module_path();
        if (strDpiResourceFile.IsEmpty())
            return _T("");

        strDpiResourceFile.Append(DPIRESOURCE_PATH);
        return strDpiResourceFile;
    }

    DWORD get_debug_dpi()
    {
        if (!is_supported_process())
            return BKWNDPI_NORMAL;

        if (m_iDebugDpi == -1)
            m_iDebugDpi = ::GetPrivateProfileInt(DPISETTING_GROUP, DPISETTING_DEBUGDPI, BKWNDPI_NORMAL, get_dpi_setting_file());
        return m_iDebugDpi;
    }

    BOOL is_debug_mode()
    {
        if (m_bDebug == -1)
            m_bDebug = ::GetPrivateProfileInt(DPISETTING_GROUP, DPISETTING_DEBUG, FALSE, get_dpi_setting_file());
        return m_bDebug;
    }

    CString get_dpi_support_process_list()
    {
        if (m_strSupportProcessList.IsEmpty())
        {
            ::GetPrivateProfileString(DPISETTING_GROUP, DPISETTING_SUPPORT_PROCESS, _T("kxetray"), m_strSupportProcessList.GetBuffer(MAX_PATH), MAX_PATH, get_dpi_setting_file());
            m_strSupportProcessList.ReleaseBuffer();
        }
        return m_strSupportProcessList;
    }

private:
    BOOL m_bInit;
    BOOL m_bDebug;
    int m_iDebugDpi;
    BOOL m_bSwitch;
    DWORD m_dwCurDpi;
    CString m_strSupportProcessList;
    enDpiMode m_iDpiMode;
    SupportDpiRateVec m_vecSupportRateList;
	static int s_dpiaware_setting_on_mem;
};

class BKEnableThreadAwarenessContextWarp
{
public:
    BKEnableThreadAwarenessContextWarp(BOOL bSupport) : m_iOldContext(0), m_bSupport(bSupport){
        if (m_bSupport)
            m_iOldContext = BKWinDpiAware::GetInstance().SetThreadDpiAwarenessContext(BKWinDpiAware::DPI_AWARENESS_PER_MONITOR_AWARE_V2);
    }
    ~BKEnableThreadAwarenessContextWarp(){
        if (m_bSupport)
            BKWinDpiAware::GetInstance().SetThreadDpiAwarenessContext((BKWinDpiAware::enDpiAwarenessContext)m_iOldContext);
    }

private:
    int m_iOldContext;
    BOOL m_bSupport;
};

__declspec(selectany) int BKWinDpiAware::s_dpiaware_setting_on_mem = BKWinDpiAware::enum_dpi_aware_setting_on_mem_null;