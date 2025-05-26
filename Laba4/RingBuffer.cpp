#include "RingBuffer.hpp"
#include <iostream>
#include <cstring>

RingBuffer::RingBuffer(const char* filename, int capacity, bool create) {
    isOpen = false;
    totalSize = sizeof(BufferHeader) + sizeof(Message) * capacity;

    if (create) {
        hFile = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE,
                            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    } else {
        hFile = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE,
                            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    }

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open file." << std::endl;
        exit(1);
    }

    SetFilePointer(hFile, totalSize, NULL, FILE_BEGIN);
    SetEndOfFile(hFile);
    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

    hMap = CreateFileMappingA(hFile, NULL, PAGE_READWRITE, 0, totalSize, NULL);
    if (!hMap) {
        std::cerr << "Failed to create file mapping." << std::endl;
        exit(1);
    }

    pView = (char*)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, totalSize);
    if (!pView) {
        std::cerr << "Failed to map view of file." << std::endl;
        exit(1);
    }

    header = (BufferHeader*)pView;
    messages = (Message*)(pView + sizeof(BufferHeader));

    if (create) {
        header->head = 0;
        header->tail = 0;
        header->count = 0;
        header->capacity = capacity;
    }

    hMutex = OpenMutexA(SYNCHRONIZE, FALSE, "Global\\RingBufferMutex");
    if (!hMutex) {
        hMutex = CreateMutexA(NULL, FALSE, "Global\\RingBufferMutex");
        if (!hMutex) {
            std::cerr << "Failed to create mutex." << std::endl;
            exit(1);
        }
    }
    hSend = OpenEventA(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, "Global\\RingBufferSendEvent");
    if (!hSend) {
        hSend = CreateEventA(NULL, FALSE, FALSE, "Global\\RingBufferSendEvent");
        if (!hSend) {
            std::cerr << "Failed to create send event." << std::endl;
            exit(1);
        }
    }

    hReceive = OpenEventA(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, "Global\\RingBufferReceiveEvent");
    if (!hReceive) {
        hReceive = CreateEventA(NULL, FALSE, FALSE, "Global\\RingBufferReceiveEvent");
        if (!hReceive) {
            std::cerr << "Failed to create receive event." << std::endl;
            exit(1);
        }
    }

    hNotEmpty = OpenEventA(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, "Global\\RingBufferEmptyEvent");
    if (!hNotEmpty) {
        hNotEmpty = CreateEventA(NULL, FALSE, FALSE, "Global\\RingBufferEmptyEvent");
        if (!hNotEmpty) {
            std::cerr << "Failed to create empty event." << std::endl;
            exit(1);
        }
    }

    hNotFull = OpenEventA(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, "Global\\RingBufferFullEvent");
    if (!hNotFull) {
        hNotFull = CreateEventA(NULL, FALSE, TRUE, "Global\\RingBufferFullEvent");
        if (!hNotFull) {
            std::cerr << "Failed to create full event." << std::endl;
            exit(1);
        }
    }
    isOpen = true;
}

bool RingBuffer:: isOpened() {
    return isOpen;
}

RingBuffer::~RingBuffer() {
    if (isOpen) {
        UnmapViewOfFile(pView);
        CloseHandle(hMap);
        CloseHandle(hFile);
        CloseHandle(hMutex);
        CloseHandle(hSend);
        CloseHandle(hReceive);
        CloseHandle(hNotFull);
        CloseHandle(hNotEmpty);
    }
}

bool RingBuffer::writeMessage(const Message& msg) {
    if (!isOpen)
        return false;

    while(true) {
        WaitForSingleObject(hMutex, INFINITE);
        if(header->count < header->capacity) {
            messages[header->tail] = msg;
            header->tail = (header->tail + 1) % header->capacity;
            header->count++;
            updateFullEmpty();
            ReleaseMutex(hMutex);
            return true;
        }
        ReleaseMutex(hMutex);
        std::cout << "Waiting for receiver reading...";
        WaitForSingleObject(hNotFull, INFINITE);
    }
}

bool RingBuffer::readMessage(Message& msg, DWORD timeout) {
    if (!isOpen)
        return false;
    bool ans = false;
    while(true) {
        WaitForSingleObject(hMutex, INFINITE);
        if(header->count > 0) {
            msg = messages[header->head];
            header->head = (header->head + 1) % header->capacity;
            header->count--;
            updateFullEmpty();
            ReleaseMutex(hMutex);
            return true;
        }
        ReleaseMutex(hMutex);
        std::cout << "Waiting for sender's message...\n";
        DWORD result = WaitForSingleObject(hNotEmpty, timeout);

        //for tests
        if (timeout != INFINITE) {
            if (result == WAIT_OBJECT_0)
                ans = true;
            else
                ans = false;
            break;
        }

    }
    //for tests
    return ans;
}