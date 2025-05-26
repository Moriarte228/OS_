//
// Created by tvysh on 5/18/2025.
//

#ifndef OS_MYEVENT_H
#define OS_MYEVENT_H

#include <windows.h>

class MyEvent {
private:
    HANDLE hEvent;
    bool ownsHandle;
    MyEvent(const MyEvent&);
    MyEvent& operator=(const MyEvent&);
    static std::vector<HANDLE> extractHandles(std::vector<MyEvent>& vec) {
        std::vector<HANDLE> rez(vec.size());
        for (size_t i = 0; i < vec.size(); ++i) {
            rez[i] = vec[i].hEvent;
        }
        return rez;
    }
public:
    MyEvent(): hEvent(NULL), ownsHandle(false) {}
    void close() {
        if (hEvent && ownsHandle) {
            CloseHandle(hEvent);
        }
        hEvent = NULL;
        ownsHandle = false;
    }
    bool createSimple(bool bManualReset, bool bInitialState) {
        if (this->created())
            return false;
        hEvent = CreateEventA(NULL, bManualReset, bInitialState, NULL);
        if (!hEvent)
            return false;
        ownsHandle = true;
        return true;
    }
    bool createInherit (bool bManualReset, bool bInitialState) {
        if (this->created())
            return false;
        SECURITY_ATTRIBUTES sa{};
        sa.nLength = sizeof(sa);
        sa.bInheritHandle = TRUE;
        sa.lpSecurityDescriptor = NULL;
        hEvent = CreateEventA(&sa, bManualReset, bInitialState, NULL);
        if (!hEvent)
            return false;
        ownsHandle = true;
        return true;
    }
    bool createReinterpret (uintptr_t rawHandle) {
        if (this->created())
            return false;
        hEvent = reinterpret_cast <HANDLE> (rawHandle);
        ownsHandle = false;
        if (!hEvent)
            return false;
        return true;
    }
    uintptr_t getHandleUintptr_t () const{
        if (!hEvent) {
            return 0;
        }
        return reinterpret_cast<uintptr_t>(hEvent);
    }
    bool operator!() const {
        return !hEvent;
    }
    bool created() const{
        return hEvent != NULL;
    }
    bool setEvent() const{
        return created() && SetEvent(hEvent);
    }
    bool resetEvent() const {
        return created() && ResetEvent(hEvent);
    }
    HANDLE getHandle() const {
        return hEvent;
    }

    DWORD waitSingleObject(DWORD timeout = INFINITE) {
        return WaitForSingleObject(hEvent, timeout);
    }

    static DWORD waitMultipleObjects(DWORD nCount, std::vector<MyEvent>& lpHandles, bool bWaitAll, DWORD timeout = INFINITE) {
        return WaitForMultipleObjects(nCount, MyEvent::extractHandles(lpHandles).data(), bWaitAll, timeout);
    }

    ~MyEvent() {
        if (hEvent && ownsHandle) {
            CloseHandle(hEvent);
        }
        hEvent = NULL;
    }

};

#endif //OS_MYEVENT_H
