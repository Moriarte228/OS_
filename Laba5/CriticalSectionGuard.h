//
// Created by tvysh on 5/28/2025.
//

#ifndef OS_CRITICALSECTIONGUARD_H
#define OS_CRITICALSECTIONGUARD_H

#include <windows.h>

class CriticalSectionGuard {
    CRITICAL_SECTION cs;

    CriticalSectionGuard(const CriticalSectionGuard&);
    CriticalSectionGuard& operator=(const CriticalSectionGuard&);

public:
    CriticalSectionGuard() { InitializeCriticalSection(&cs); }
    ~CriticalSectionGuard() { DeleteCriticalSection(&cs); }

    void lock() { EnterCriticalSection(&cs); }
    void unlock() { LeaveCriticalSection(&cs); }

    CRITICAL_SECTION* get() { return &cs; }
};

#endif //OS_CRITICALSECTIONGUARD_H
