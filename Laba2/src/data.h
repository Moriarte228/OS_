#ifndef ADD_H
#define ADD_H

#include <iostream>
#include <vector>
#include <windows.h>

#include "CriticalSectionGuard.h"

constexpr int  SLEEP_MIN_MAX = 7;
constexpr int SLEEP_AVERAGE = 12;

struct SharedData {
    int min_val;
    int max_val;
    double average;
    std::vector<int> arr;
    CriticalSectionGuard cs;
    SharedData() : min_val(0), max_val(0), average(0.0), arr(), cs() {}
private:
    SharedData(const SharedData&) = delete;
    SharedData& operator=(const SharedData&) = delete;
};


#endif