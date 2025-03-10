#include <windows.h>
#include "functions.h"

DWORD WINAPI compute_min_max(LPVOID param) {
    SharedData* data = static_cast<SharedData*>(param);
    data->cs.lock();
    int min = data->arr[0];
    int max = data->arr[0];
    data->cs.unlock();

    for (size_t i = 1; i < data->arr.size(); i++) {
        data->cs.lock();
        int val = data->arr[i];
        data->cs.unlock();

        if (val > max) {
            max = val;
            Sleep(SLEEP_MIN_MAX);
        }
        if (val < min) {
            min = val;
            Sleep(SLEEP_MIN_MAX);
        }
    }

    data->cs.lock();
    data->min_val = min;
    data->max_val = max;
    data->cs.unlock();
    return 0;
}

DWORD WINAPI compute_average(LPVOID param) {
    SharedData* data = static_cast<SharedData*>(param);
    int sum = 0;

    for (size_t i = 0; i < data->arr.size(); ++i) {
        data->cs.lock();
        int val = data->arr[i];
        data->cs.unlock();

        sum += val;
        Sleep(SLEEP_AVERAGE);
    }

    data->cs.lock();
    data->average = static_cast<double>(sum) / data->arr.size();
    data->cs.unlock();
    return 0;
}

void update_array(SharedData& data) {
    for (size_t i = 0; i < data.arr.size(); ++i) {
        int& val = data.arr[i];
        if (val == data.min_val || val == data.max_val) {
            val = static_cast<int>(data.average);
        }
    }
}

void print_array(const SharedData& data) {
    std::cout << "Changed array: ";
    for (size_t i = 0; i < data.arr.size(); ++i) {
        std::cout << data.arr[i] << " ";
    }
    std::cout << std::endl;
}

bool input_data(SharedData& data) {
    int size;
    std::cout << "Size of array: ";
    if (!(std::cin >> size) || size <= 0) {
        std::cerr << "Error: incorrect size" << std::endl;
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return false;
    }

    data.arr.resize(size);
    std::cout << "Enter " << size << " elements: ";

    for (size_t i = 0; i < data.arr.size(); ++i) {
        if (!(std::cin >> data.arr[i])) {
            std::cerr << "Error: incorrect element" << std::endl;
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            return false;
        }
    }
    return true;
}
