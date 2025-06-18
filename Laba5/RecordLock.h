//
// Created by tvysh on 6/1/2025.
//

#ifndef OS_RECORDLOCK_H
#define OS_RECORDLOCK_H

#include <windows.h>
#include "CriticalSectionGuard.h"


class RecordLock {
private:
    HANDLE emptyModeEvent;
    HANDLE readerModeEvent;
    HANDLE writerModeEvent;
    unsigned int readersCount;
    bool hasWriter;
    CriticalSectionGuard cs;
    unsigned int preReadersCount;
public:
    RecordLock();
    ~RecordLock();
    void lockForRead();
    void lockForWrite();
    void unlockFromRead();
    void unlockFromWrite();
};


#endif //OS_RECORDLOCK_H
