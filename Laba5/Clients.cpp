// Client.cpp
#include <windows.h>
#include <iostream>
#include <string>
#include "Employee.h"
#include "MessageProtocol.h"

void printError(const char* msg) {
    std::cerr << msg << " Error: " << GetLastError() << "\n";
}

bool sendAll(HANDLE pipe, const void* data, DWORD size) {
    DWORD written;
    return WriteFile(pipe, data, size, &written, NULL) && written == size;
}

bool readAll(HANDLE pipe, void* data, DWORD size) {
    DWORD read;
    return ReadFile(pipe, data, size, &read, NULL) && read == size;
}

int main() {
    std::cout << "Enter client ID (used for pipe name): ";
    unsigned int clientId;
    std::cin >> clientId;
    std::cin.ignore();

    std::string pipeName = getPipeName(clientId);

    std::cout << "Connecting to pipe: " << pipeName << " ...\n";
    HANDLE hPipe = CreateFileA(
            pipeName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0, NULL, OPEN_EXISTING, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        printError("Failed to connect to pipe");
        return 1;
    }

    bool running = true;
    while (running) {
        std::cout << "Choose operation:\n"
                  << "1 - Modify record\n"
                  << "2 - Read record\n"
                  << "3 - Exit\n"
                  << "> ";
        int op;
        std::cin >> op;
        std::cin.ignore();

        if (op == 3) {
            // Send exit command
            char cmd = *TYPE_EXIT;
            if (!sendAll(hPipe, &cmd, 1)) {
                printError("Failed to send exit command");
            }
            running = false;
            break;
        }
        else if (op != 1 && op != 2) {
            std::cout << "Invalid operation.\n";
            continue;
        }

        // Send command
        char cmd = (op == 1) ? *TYPE_MODIFY : *TYPE_READ;
        if (!sendAll(hPipe, &cmd, 1)) {
            printError("Failed to send command");
            break;
        }

        std::cout << "Enter Employee ID: ";
        unsigned int id;
        std::cin >> id;
        std::cin.ignore();

        // Send ID to server
        if (!sendAll(hPipe, &id, sizeof(id))) {
            printError("Failed to send ID");
            break;
        }

        // Read server response for ID validity
        char errorCode[2] = {0};
        if (!readAll(hPipe, errorCode, 1)) {
            printError("Failed to read error code");
            break;
        }
        if (errorCode[0] != *ERROR_OK) {
            std::cout << "Server error: ID not found or invalid command\n";
            continue;
        }

        if (op == 1) {
            // MODIFY flow

            // Read existing Employee record from server
            Employee emp{};
            if (!readAll(hPipe, &emp, sizeof(Employee))) {
                printError("Failed to read employee record");
                break;
            }
            std::cout << "Current record:\n" << print(emp);

            // Enter new values
            std::cout << "Enter new name (max 9 chars): ";
            std::string newName;
            std::getline(std::cin, newName);
            if (newName.size() > 9) newName.resize(9);
            std::memset(emp.name, 0, sizeof(emp.name));
            std::memcpy(emp.name, newName.c_str(), newName.size());

            std::cout << "Enter new hours: ";
            std::cin >> emp.hours;
            std::cin.ignore();

            // Send modified record to server
            if (!sendAll(hPipe, &emp, sizeof(Employee))) {
                printError("Failed to send modified employee");
                break;
            }

            // Read response after modification
            if (!readAll(hPipe, errorCode, 1)) {
                printError("Failed to read modify response");
                break;
            }
            if (errorCode[0] == *ERROR_OK) {
                std::cout << "Record modified successfully.\n";
            } else {
                std::cout << "Failed to modify record. Server error code: " << errorCode[0] << "\n";
            }

            // Wait for user command to finish
            std::cout << "Press Enter to finish modification...";
            std::cin.get();

        } else if (op == 2) {
            // READ flow

            // Read server response if record exists and get employee record
            Employee emp{};
            if (!readAll(hPipe, errorCode, 1)) {
                printError("Failed to read read response");
                break;
            }
            if (errorCode[0] != *ERROR_OK) {
                std::cout << "Failed to read record. Server error code: " << errorCode[0] << "\n";
                continue;
            }
            if (!readAll(hPipe, &emp, sizeof(Employee))) {
                printError("Failed to read employee data");
                break;
            }

            std::cout << "Record data:\n" << print(emp);

            std::cout << "Press Enter to finish reading...";
            std::cin.get();
        }
    }

    CloseHandle(hPipe);
    std::cout << "Client exited.\n";
    return 0;
}
