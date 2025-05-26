#ifndef RING_BUFFER_HPP
#define RING_BUFFER_HPP

#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include "MyEvent.h"
#include "MyProcess.h"

const int MAX_MESSAGE_LENGTH = 20;

struct Message {
    char text[MAX_MESSAGE_LENGTH + 1] = {};
    Message() { std::memset(text, 0, sizeof(text)); }
    bool setText(const std::string& s) {
        if (s.length() > MAX_MESSAGE_LENGTH || s.length() == 0)
            return false;
        std::memset(text, 0, sizeof(text));
        std::strncpy(text, s.c_str(), sizeof(text) - 1);
        return true;
    }
};

struct BufferHeader {
    int head;
    int tail;
    int count;
    int capacity;
};

class RingBuffer {
public:
    RingBuffer(const char* filename, int capacity, bool create);
    ~RingBuffer();

    bool writeMessage(const Message& msg);
    bool readMessage(Message& msg, DWORD timeout = INFINITE);
    bool isOpened();
private:
    bool isOpen;
    HANDLE hFile;
    HANDLE hMap;
    char* pView;
    BufferHeader* header;
    Message* messages;
    int totalSize;
    HANDLE hMutex;
    HANDLE hSend;
    HANDLE hReceive;
    HANDLE hNotFull;
    HANDLE hNotEmpty;

    void updateFullEmpty() {
        //not empty
        if (this->header->count != 0) {
            SetEvent(hNotEmpty);
        }
        else
            ResetEvent(hNotEmpty);
        //not full
        if (this->header->count != this->header->capacity) {
            SetEvent(hNotFull);
        }
        else
            ResetEvent(hNotFull);
    }
};

#endif // RING_BUFFER_HPP