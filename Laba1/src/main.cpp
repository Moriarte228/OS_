#include "Employee.h"

int main() {
    std::string binaryFileName, reportFileName;
    int numEntries;
    double hourlyRate;

    // Запрос имени бинарного файла и количества записей
    std::cout << "Enter binary file name: ";
    std::cin >> binaryFileName;
    std::cout << "Enter number of entries: ";
    std::cin >> numEntries;

    // Запуск утилиты Creator
    std::string creatorCommand = "creator " + binaryFileName + " " + std::to_string(numEntries);

    STARTUPINFO siCreator = { sizeof(siCreator) };
    PROCESS_INFORMATION piCreator;
    if (!CreateProcess(
        nullptr,                   // Имя исполняемого файла (указывается в командной строке)
        &creatorCommand[0],        // Командная строка
        nullptr,                   // Атрибуты безопасности процесса
        nullptr,                   // Атрибуты безопасности потока
        FALSE,                     // Наследование дескрипторов
        0,                         // Флаги создания
        nullptr,                   // Окружение
        nullptr,                   // Текущий каталог
        &siCreator,                // STARTUPINFO
        &piCreator                 // PROCESS_INFORMATION
    )) {
        std::cerr << "Failed to start Creator process!" << std::endl;
        return 1;
    }

    // Ожидание завершения процесса Creator
    WaitForSingleObject(piCreator.hProcess, INFINITE);
    CloseHandle(piCreator.hProcess);
    CloseHandle(piCreator.hThread);

    // Запрос имени файла отчёта и оплаты за час работы
    std::cout << "Enter report file name: ";
    std::cin >> reportFileName;
    std::cout << "Enter hourly rate: ";
    std::cin >> hourlyRate;

    // Запуск утилиты Reporter
    std::string reporterCommand = "reporter " + binaryFileName + " " + reportFileName + " " + std::to_string(hourlyRate);

    STARTUPINFO siReporter = { sizeof(siReporter) };
    PROCESS_INFORMATION piReporter;
    if (!CreateProcess(
        nullptr,                   // Имя исполняемого файла (указывается в командной строке)
        &reporterCommand[0],       // Командная строка
        nullptr,                   // Атрибуты безопасности процесса
        nullptr,                   // Атрибуты безопасности потока
        FALSE,                     // Наследование дескрипторов
        0,                         // Флаги создания
        nullptr,                   // Окружение
        nullptr,                   // Текущий каталог
        &siReporter,               // STARTUPINFO
        &piReporter                // PROCESS_INFORMATION
    )) {
        std::cerr << "Failed to start Reporter process!" << std::endl;
        return 1;
    }

    // Ожидание завершения процесса Reporter
    WaitForSingleObject(piReporter.hProcess, INFINITE);
    CloseHandle(piReporter.hProcess);
    CloseHandle(piReporter.hThread);

    // Вывод отчёта на консоль
    std::ifstream reportFile(reportFileName);
    if (!reportFile) {
        std::cerr << "Cannot open report file!" << std::endl;
        return 1;
    }

    std::cout << "\nReport from file \"" << reportFileName << "\":" << std::endl;
    std::string line;
    while (std::getline(reportFile, line)) {
        std::cout << line << std::endl;
    }

    reportFile.close();

    return 0;
}
