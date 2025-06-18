//
// Created by tvysh on 6/1/2025.
//

#include "RecordLock.h"

RecordLock::RecordLock(): readersCount(0), preReadersCount(0), hasWriter(false), cs(){
    emptyModeEvent = CreateEventA(NULL, FALSE, TRUE, NULL);
    readerModeEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
    writerModeEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
}

RecordLock::~RecordLock() {
    CloseHandle(emptyModeEvent);
    CloseHandle(readerModeEvent);
    CloseHandle(writerModeEvent);
}

void RecordLock::lockForRead() {
    cs.lock();
    preReadersCount++;
    cs.unlock();
    HANDLE myHandles[2] {emptyModeEvent, readerModeEvent};
    WaitForMultipleObjects(2, myHandles, FALSE, INFINITE);
    cs.lock();
    preReadersCount--;
    readersCount++;
    SetEvent(readerModeEvent);
    cs.unlock();
}

void RecordLock::unlockFromRead() {
    cs.lock();
    readersCount--;
    if ((readersCount == 0) && (preReadersCount == 0)) {
        ResetEvent(readerModeEvent);
        SetEvent((emptyModeEvent));
    }
    cs.unlock();
}

void RecordLock::lockForWrite() {
    WaitForSingleObject(emptyModeEvent, INFINITE);
    cs.lock();
    hasWriter = true;
    SetEvent(writerModeEvent);
    cs.unlock();
}

void RecordLock::unlockFromWrite() {
    cs.lock();
    hasWriter = false;
    ResetEvent(writerModeEvent);
    SetEvent(emptyModeEvent);
    cs.unlock();
}
