#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <vector>
#include <windows.h>
#include "data.h"

// Объявления функций (без реализации)
DWORD WINAPI compute_min_max(LPVOID param);
DWORD WINAPI compute_average(LPVOID param);
void update_array(SharedData &data);
void print_array(const SharedData &data);
bool input_data(SharedData &data);

#endif  // FUNCTIONS_H
