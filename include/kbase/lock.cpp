#include "stdafx.h"
#include "precompile_macros.h"
#include "lock.h"
namespace kbase
{
    Locker::Locker()
    {
        // The second parameter is the spin count, for short-held locks it avoid the
        // contending thread from going to sleep which helps performance greatly.
        ::InitializeCriticalSectionAndSpinCount(&sec, 2000);
    }
    bool Locker::Try()
    {
        if (::TryEnterCriticalSection(&sec) != FALSE)
        {
            return true;
        }
        return false;
    }
}