
#include "ThreadController.h"

unsigned int ThreadController::markerCount = 0;
HANDLE ThreadController::startEvent = NULL;
HANDLE ThreadController::cannotProceedEvents[ThreadController::MAX_MARKERS];
HANDLE ThreadController::continueEvents[ThreadController::MAX_MARKERS];
HANDLE ThreadController::terminateEvents[ThreadController::MAX_MARKERS];
HANDLE ThreadController::markerThreads[ThreadController::MAX_MARKERS];
HANDLE ThreadController::threadIsTerminated[ThreadController::MAX_MARKERS];
std::set<int> ThreadController::workingThreads;
Data* ThreadController::data = NULL;
CRITICAL_SECTION  ThreadController:: coutSection;

ThreadController::ThreadController(unsigned int markers, Data* v_data) {
    InitializeCriticalSection(&coutSection);

    if (markers > MAX_MARKERS) {
        throw ("Too many markers");
    }
    data = v_data;
    markerCount = markers;

    startEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    for (int i = 1; i <= markerCount; ++i) {
        cannotProceedEvents[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
        continueEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        threadIsTerminated[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        terminateEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
    }

    for (int i = 1; i <= markerCount; ++i) {
        workingThreads.insert(i);
        markerThreads[i] = CreateThread(
                NULL,
                0,
                MarkerThread,
                (LPVOID)(INT_PTR)i,
                0,
                NULL
        );
    }
}

ThreadController::~ThreadController() {
    
    CloseHandle(startEvent);

    for (int i = 0; i < markerCount; ++i) {
        WaitForSingleObject(markerThreads[i], INFINITE);
        CloseHandle(cannotProceedEvents[i]);
        CloseHandle(continueEvents[i]);
        CloseHandle(terminateEvents[i]);
        CloseHandle(threadIsTerminated[i]);
        CloseHandle(markerThreads[i]);

    }
    DeleteCriticalSection(&coutSection);
}

void ThreadController::StartEvent() {
    Sleep(500);
    SetEvent(startEvent);
    Sleep(500);
    ResetEvent(startEvent);
}

void ThreadController::CycleEvent() {
    std::stringstream lStream;
    while (!workingThreads.empty()) {
        WaitForMultipleObjects(markerCount, &cannotProceedEvents[0], TRUE, INFINITE);

        lStream << data->print();
        print(lStream);

        unsigned int id;
        while (true) {
            lStream << "Enter id marker to finish this marker: ";
            print(lStream);

            std::cin >> id;

            if (workingThreads.find(id) != workingThreads.end())
                break;

            EnterCriticalSection(&coutSection);
            lStream << "Incorrect or invalid id, try again\n";
            print(lStream);
            LeaveCriticalSection(&coutSection);
        }

        FinishThread(id);

        ContinueThreads();
    }
}

void ThreadController::FinishThread(unsigned int id) {
    workingThreads.erase(id);
    SetEvent(terminateEvents[id]);
    WaitForSingleObject(threadIsTerminated[id], INFINITE);
}

void ThreadController::ContinueThreads() {
    for (auto id : workingThreads) {
        ResetEvent(cannotProceedEvents[id]);
    }
    for (auto id : workingThreads) {
        SetEvent(continueEvents[id]);
    }
}

void ThreadController::print(std::stringstream& ss) {
    EnterCriticalSection(&coutSection);
    std::cout << ss.str();
    std::cout.flush();
    LeaveCriticalSection(&coutSection);
    ss.str("");
    ss.clear();
}

DWORD ThreadController::MarkerThread(LPVOID lpParam) {
    int id = (int)(INT_PTR)lpParam;  // Получаем ID потока
    std::stringstream lStream;
    WaitForSingleObject(startEvent, INFINITE);
    lStream << "Marker thread " << id << " started.\n";
    print(lStream);
    int totalMarked = 0;
    srand(id);
    HANDLE myEvents[2] {continueEvents[id], terminateEvents[id]};

    while (true) {
        size_t index = rand() % data->getSize();

        if (data->get(index) == 0) {
            Sleep(5);
            data->set(index, id);
            totalMarked++;
            Sleep(5);
        } else {
            lStream << "Marker thread " << id << ": Unable to mark index " << index << ".\n"
            << "Total marked elements: " << totalMarked << ".\n"
            << "Element " << index << " is already marked.\n";
            print(lStream);

            SetEvent(cannotProceedEvents[id]);

            DWORD waitResult = WaitForMultipleObjects(2, myEvents, FALSE, INFINITE);

            if (waitResult == WAIT_OBJECT_0){
                lStream << "Marker thread " << id << " continues work.\n";
                print(lStream);
                continue;
            }
            else if (waitResult == WAIT_OBJECT_0 + 1) {
                lStream << "Marker thread " << id << " will terminate.\n";
                print(lStream);
                break;
            }
            else {
                lStream << "Unexpected wait result.\n";
                print(lStream);
                break;
            }
        }
    }

    data->clearMarksBy(id);

    SetEvent(threadIsTerminated[id]);
    SetEvent(cannotProceedEvents[id]);
    lStream << "Marker thread " << id << " finished.\n";
    print(lStream);
    return 0;
}
