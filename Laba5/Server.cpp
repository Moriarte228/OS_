//
// Server.cpp
//

#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include "Employee.h"
#include "ServerController.h"
#include "MessageProtocol.h"

void printEmployeesFromFile(const char* filename) {
    std::ifstream fin(filename, std::ios::binary);
    if (!fin) {
        std::cerr << "Failed to open file for reading.\n";
        return;
    }
    std::cout << "\nContents of file '" << filename << "':\n";
    Employee e;
    while (fin.read(reinterpret_cast<char*>(&e), sizeof(Employee))) {
        std::cout << print(e);
    }
    fin.close();
}

int main() {
    std::string filename;
    std::cout << "Enter filename for employee records: ";
    std::getline(std::cin, filename);

    std::cout << "Enter number of employees to add: ";
    int n;
    std::cin >> n;
    std::cin.ignore();

    std::cout << "Enter number of client processes to run: ";
    unsigned int numClients;
    std::cin >> numClients;
    std::cin.ignore();

    // Инициализация контроллера
    ServerController serverController(numClients, n, filename.c_str(), n);

    // Добавляем записи сотрудников
    for (int i = 0; i < n; ++i) {
        Employee e{};
        std::cout << "Employee #" << (i + 1) << "\n";

        std::cout << "ID: ";
        std::cin >> e.id;
        std::cin.ignore();

        std::cout << "Name (max 9 chars): ";
        std::string name;
        std::getline(std::cin, name);
        if (name.length() > 9)
            name = name.substr(0, 9);
        std::strncpy(e.name, name.c_str(), sizeof(e.name));
        e.name[9] = '\0';

        std::cout << "Hours: ";
        std::cin >> e.hours;
        std::cin.ignore();

        if (!serverController.addRecord(e)) {
            std::cerr << "Failed to add employee record.\n";
        }
    }

    // Выводим созданный файл
    printEmployeesFromFile(filename.c_str());


    // Инициализируем ServerController с нужными параметрами
    // Передаём количество клиентов и количество записей
    ServerController server(numClients, n, filename.c_str(), n);

    // Создаём и запускаем клиентские потоки на сервере (обслуживание клиентов)
    if (!server.createThreads()) {
        std::cerr << "Failed to create client threads.\n";
        return 1;
    }

    std::cout << "Server is running and serving clients...\n";

    // Ожидаем, пока все клиентские потоки завершатся
    WaitForSingleObject(server.noActiveClientThreads, INFINITE);

    std::cout << "All client threads finished.\n";

    // Выводим итоговое состояние файла после работы клиентов
    printEmployeesFromFile(filename.c_str());

    return 0;
}
