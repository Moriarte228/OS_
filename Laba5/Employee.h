#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <windows.h>
#include <cstdlib>
#include <map>
#include <atomic>
#include <vector>
#include <queue>
#include <thread>
#include "CriticalSectionGuard.h"
#include "MessageProtocol.h"

#pragma pack(push, 1)
struct Employee {
    unsigned int id;
    char name[10];
    double hours;
};
#pragma pack(pop)

inline std::string print(const Employee& e) {
    return "ID: " + std::to_string(e.id) +
           " Name: " + std::string(e.name) +
           " Hours: " + std::to_string(e.hours) + "\n";
}


#endif // EMPLOYEE_H