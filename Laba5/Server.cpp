
#include <iostream>
#include <string>
#include <limits>
#include "ServerController.h"

int main() {
    std::cout << "Enter file name: ";
    std::string filename;
    std::getline(std::cin, filename);

    std::cout << "Enter number of employee records: ";
    unsigned int numRecords;
    std::cin >> numRecords;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Создаем сервер с вместимостью numRecords и столько клиентов, сколько захотим запустить
    std::cout << "Enter number of client processes to launch: ";
    unsigned int numClients;
    std::cin >> numClients;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    ServerController serverController(numClients, numRecords, filename.c_str(), numRecords);

    // Ввод сотрудников
    for (unsigned int i = 0; i < numRecords; ++i) {
        unsigned int id;
        std::string tempName;
        double hours;

        std::cout << "Enter employee ID: ";
        std::cin >> id;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "Enter employee name (max 9 characters): ";
        std::getline(std::cin, tempName);

        if (tempName.length() > 9) {
            std::cout << "Name too long, truncating to 9 characters.\n";
            tempName = tempName.substr(0, 9);
        }

        std::cout << "Enter hours worked: ";
        std::cin >> hours;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        Employee emp;
        emp.id = id;
        strncpy(emp.name, tempName.c_str(), 9);
        emp.name[9] = '\0';
        emp.hours = hours;

        if (!serverController.addRecord(emp)) {
            std::cerr << "Failed to add record for employee ID " << id << "\n";
        }
    }

    std::cout << "\nInitial employee records:\n";
    std::string fileContent;
    std::cout << serverController.printFile();

    std::cout << "\nStarting client threads...\n";
    if (!serverController.createThreads()) {
        std::cerr << "Failed to create client threads.\n";
        return 1;
    }

    std::cout << "Clients are running.\n";

    serverController.waitForClientsThreadsEnd();

    std::cout << "\nFinal employee records after client modifications:\n";

   std::cout << serverController.printFile();

    std::cout << "Server shutting down.\n";

    return 0;
}
