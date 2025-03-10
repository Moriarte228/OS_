#include "./src/data.h"
#include "./src/functions.h"
#include "./src/CriticalSectionGuard.h"

int main() {
    SharedData data;

    if (!input_data(data)) return 1;

    // Создание потоков
    HANDLE threadMinMax = CreateThread(NULL, 0, compute_min_max, &data, 0, NULL);
    HANDLE threadAverage = CreateThread(NULL, 0, compute_average, &data, 0, NULL);

    if (!threadMinMax || !threadAverage) {
        std::cerr << "Error:failed to create threads" << std::endl;
        return 1;
    }

    // Ожидание завершения потоков
    WaitForSingleObject(threadMinMax, INFINITE);
    WaitForSingleObject(threadAverage, INFINITE);

    CloseHandle(threadMinMax);
    CloseHandle(threadAverage);

    // Обновление массива и вывод результата
    update_array(data);
    print_array(data);

    return 0;
}