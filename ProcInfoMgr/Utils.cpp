#include "Utils.h"
#include <math.h>

ULONGLONG Utils::FileTimeToULONGLONG(const FILETIME& ftFileTime)
{
    const ULONGLONG uHightPartPow = (ULONGLONG)pow((double)2, 32);
    return (ULONGLONG)ftFileTime.dwHighDateTime * uHightPartPow + (ULONGLONG)ftFileTime.dwLowDateTime;
}

FILETIME Utils::ULONGLONGToFileTime(ULONGLONG uLongLongTime)
{
    FILETIME ftFunRet = {0};

    ftFunRet.dwHighDateTime = (DWORD)(uLongLongTime >> 32);
    ftFunRet.dwLowDateTime = (DWORD)(uLongLongTime & (ULONGLONG)0x00000000FFFFFFFF);

    return ftFunRet;
}

ULONGLONG Utils::GetTimezoneNanoSeconds()
{
    ULONGLONG uFunRet = 0;
    TIME_ZONE_INFORMATION tzi = {0};
    SYSTEMTIME stTemp = {0};
    DWORD dwRet = 0;
    LONG lBias = 0;

    dwRet = ::GetTimeZoneInformation(&tzi);
    if (dwRet == TIME_ZONE_ID_INVALID)
        return 0;

    if (memcmp(&stTemp, &tzi.DaylightDate, sizeof(SYSTEMTIME)) == 0)
        lBias = tzi.Bias;
    else
        lBias = tzi.Bias + tzi.DaylightBias;

    uFunRet = (ULONGLONG)(-1 * lBias * 60) * (ULONGLONG)10000000;

    return uFunRet;
}

