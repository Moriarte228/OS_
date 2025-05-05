#ifndef DATA_H
#define DATA_H

#include <windows.h>
#include <cstring>  // для memset
#include <iostream>
#include <set>
#include <sstream>
#include <ctime>

class Data {
private:
    int* arr;
    size_t size;
    CRITICAL_SECTION cs;

public:
    Data(size_t n) : size(n) {
        arr = new int[size];
        std::memset(arr, 0, sizeof(int) * size);
        InitializeCriticalSection(&cs);
    }

    ~Data() {
        DeleteCriticalSection(&cs);
        delete[] arr;
    }

    void set(size_t index, int value) {
        EnterCriticalSection(&cs);
        Sleep(5);
        if (index < size) {
            arr[index] = value;
        }
        Sleep(5);
        LeaveCriticalSection(&cs);
    }

    int get(size_t index) {
        EnterCriticalSection(&cs);
        int result = 0;
        if (index < size) {
            result = arr[index];
        }
        LeaveCriticalSection(&cs);
        return result;
    }

    size_t getSize() const {
        return size;
    }

    std::string print(){
        EnterCriticalSection(&cs);
        std::stringstream ss;
        for (size_t i = 0; i < size; ++i) {
            ss << arr[i] << " ";
        }
        ss << "\n";
        LeaveCriticalSection(&cs);
        return ss.str();
    }

    void clearMarksBy(int markerId) {
        EnterCriticalSection(&cs);
        for (size_t i = 0; i < size; ++i) {
            if (arr[i] == markerId) {
                arr[i] = 0;
            }
        }
        LeaveCriticalSection(&cs);
    }
};

#endif
