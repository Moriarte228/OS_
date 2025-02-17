#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <cstdlib>

struct employee {
    int num;        // идентификационный номер сотрудника
    char name[10];  // имя сотрудника
    double hours;   // количество отработанных часов
};

#endif // EMPLOYEE_H