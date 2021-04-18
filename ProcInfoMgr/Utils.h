#pragma once
#include "stdafx.h"

class Utils
{
public:
    static ULONGLONG FileTimeToULONGLONG(const FILETIME& ftFileTime);
    static FILETIME ULONGLONGToFileTime(ULONGLONG uLongLongTime);
    static ULONGLONG GetTimezoneNanoSeconds();
};