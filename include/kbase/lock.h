///////////////////////////////////////////////////////////////
// @file            lock.h
// @author          gancan <gancan@kingsoft.com>
// @date            2013-07-12  10:45
// @brief          get source code from chromium
///////////////////////////////////////////////////////////////

#ifndef KBASE_SYNCHRONIZATION_LOCK_H_
#define KBASE_SYNCHRONIZATION_LOCK_H_
#include "utildefine.h"
#include <windows.h>

namespace kbase
{
    class EmptyLocker
    {
    public:
        void Lock()
        {
        }
        void Unlock()
        {
        }
    };

    class Locker
    {
    public:
        Locker();
        ~Locker()
        {
            ::DeleteCriticalSection(&sec);
        }
        void Lock()
        {
            ::EnterCriticalSection(&sec);
        }
        void Unlock()
        {
            ::LeaveCriticalSection(&sec);
        }
        bool Try();

    private:
        CRITICAL_SECTION sec;
        K_DISALLOW_COPY_AND_ASSIGN (Locker);
    };

    class AutoLock
    {
    public:
        explicit AutoLock(Locker& lock) :
                        lock_(lock)
        {
            lock_.Lock();
        }

        ~AutoLock()
        {
            lock_.Unlock();
        }

    private:
        Locker& lock_;
        K_DISALLOW_COPY_AND_ASSIGN (AutoLock);
    };

    class AutoUnlock
    {
    public:
        explicit AutoUnlock(Locker& lock) :
                        lock_(lock)
        {
            lock_.Unlock();
        }

        ~AutoUnlock()
        {
            lock_.Lock();
        }

    private:
        Locker& lock_;
        K_DISALLOW_COPY_AND_ASSIGN (AutoUnlock);
    };

}  // namespace kbase

#endif	//KBASE_SYNCHRONIZATION_LOCK_H_
