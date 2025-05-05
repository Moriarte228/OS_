//
// Created by tvysh on 4/28/2025.
//

#ifndef OS_THREADCONTROLLER_H
#define OS_THREADCONTROLLER_H

#include "Data.h"


class ThreadController {
    static const unsigned int MAX_MARKERS = 64;
    static unsigned int markerCount;
    static HANDLE startEvent;
    static HANDLE cannotProceedEvents[MAX_MARKERS];
    static HANDLE continueEvents[MAX_MARKERS];
    static HANDLE terminateEvents[MAX_MARKERS];
    static HANDLE markerThreads[MAX_MARKERS];
    static HANDLE threadIsTerminated[MAX_MARKERS];
    static std::set<int> workingThreads;
    static CRITICAL_SECTION coutSection;
public:
    static Data* data;
    ThreadController(unsigned int markers, Data* data);
    ~ThreadController();
    static unsigned int gMAX_MARKERS() {
        return MAX_MARKERS;
    }
    static void StartEvent();
    static void CycleEvent();
    static void FinishThread(unsigned int id);
    static void ContinueThreads();
    static DWORD WINAPI MarkerThread(LPVOID lpParam);
    static void print(std::stringstream& ss);
};


#endif //OS_THREADCONTROLLER_H
