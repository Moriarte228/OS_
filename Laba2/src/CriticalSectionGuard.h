class CriticalSectionGuard {
    CRITICAL_SECTION cs;

public:
    CriticalSectionGuard() { InitializeCriticalSection(&cs); }
    ~CriticalSectionGuard() { DeleteCriticalSection(&cs); }

    CriticalSectionGuard(const CriticalSectionGuard&) = delete;
    CriticalSectionGuard& operator=(const CriticalSectionGuard&) = delete;

    void lock() { EnterCriticalSection(&cs); }
    void unlock() { LeaveCriticalSection(&cs); }

    CRITICAL_SECTION* get() { return &cs; }
};