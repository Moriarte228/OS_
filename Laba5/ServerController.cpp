//
// Created by tvysh on 5/28/2025.
//

#include "ServerController.h"

ServerController::ServerController(unsigned int clients, unsigned int records, const char *filename,
                                   unsigned int capacity): numberOfClients(clients), numberOfRecords(records),
                                                           fileController(filename, capacity), recordLocks(records),
                                                           activeClientThreads(0), clientServerPipes(clients + 1){ // +1 чтобы индекс совпадал с id
    noActiveClientThreads = CreateEventA(NULL, FALSE, FALSE, NULL);
}

ServerController::~ServerController(){
    CloseHandle(noActiveClientThreads);
}

bool ServerController::addRecord(Employee emp) {
    return fileController.addRecord(emp);
}

bool ServerController::createThreads() {
    ResetEvent(noActiveClientThreads);

    for (unsigned int i = 1; i <= numberOfClients; ++i) {
        ++activeClientThreads;
        std::thread t([this, i]() {
            this->clientThread(i);
            if (--activeClientThreads == 0) {
                SetEvent(noActiveClientThreads); // Event of no client threads live
            }
        });
        t.detach();
    }

    return true;
}

void ServerController::startClientProcess(unsigned int id) {
    std::string cmdLine = "Client.exe " + std::to_string(id); // клиент получает свой id как аргумент

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Запуск процесса клиента
    BOOL success = CreateProcessA(
            NULL,                   // имя исполняемого файла (NULL — значит использовать из cmdLine)
            &cmdLine[0],            // командная строка (важно — не const char*, а char*)
            NULL,                   // атрибуты процесса
            NULL,                   // атрибуты потока
            FALSE,                  // наследование дескрипторов
            CREATE_NEW_CONSOLE,                      // флаги создания
            NULL,                   // окружение
            NULL,                   // рабочий каталог
            &si,                    // информация о запуске
            &pi                    // информация о процессе
    );

    if (!success) {
        std::cerr << "Failed to start client process " << id
                  << ". Error: " << GetLastError() << std::endl;
    } else {
        // Можно закрыть дескрипторы — клиент работает сам по себе
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

HANDLE ServerController::createAndConnectPipe(unsigned int id) {
    std::string pipeName = CLIENT_SERVER_PIPE_BASE + std::to_string(id);


    HANDLE hPipe = CreateNamedPipeA(pipeName.c_str(),
                                    PIPE_ACCESS_DUPLEX,
                                    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                                    1, 1024, 1024, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateNamedPipe " << id << " failed. Error: " << GetLastError() << "\n";
        return INVALID_HANDLE_VALUE;
    }

    startClientProcess(id);

    BOOL connected = ConnectNamedPipe(hPipe, NULL) ?
                     TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

    if (!connected) {
        std::cerr << "ConnectNamedPipe " << id << " failed. Error: " << GetLastError() << "\n";
        CloseHandle(hPipe);
        return INVALID_HANDLE_VALUE;
    }

    return hPipe;
}

void ServerController::sendError(HANDLE hPipe, const char* error) {
    DWORD bytesWritten;
    if (!WriteFile(hPipe, error, strlen(error), &bytesWritten, NULL)) {
        std::cerr << "sendError WriteFile failed: " << GetLastError() << "\n";
    }
}

const char* ServerController::readCommand(HANDLE hPipe) {
    char buffer[128];
    DWORD bytesRead;

    std::cout << "DEBUG: Waiting for command from client..." << std::endl;
    if (!ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) || bytesRead == 0) {
        std::cerr << "readCommand: Failed to read or client disconnected. Error: " << GetLastError() << std::endl;
        return nullptr;
    }
    std::cout << "DEBUG: Received " << bytesRead << " bytes, command: " << buffer[0] << std::endl;

    if (bytesRead != 1) {
        std::cerr << "readCommand: Invalid command length: " << bytesRead << std::endl;
        DWORD written;
        WriteFile(hPipe, ERROR_UNKNOWN_COMMAND, 1, &written, NULL);  // Отправляем 1 байт
        return nullptr;
    }

    buffer[bytesRead] = '\0';

    switch (buffer[0]) {
        case 'M':  // *TYPE_MODIFY
            std::cout << "Command received: MODIFY" << std::endl;
            return TYPE_MODIFY;
        case 'R':  // *TYPE_READ
            std::cout << "Command received: READ" << std::endl;
            return TYPE_READ;
        case 'E':  // *TYPE_EXIT
            std::cout << "Command received: EXIT" << std::endl;
            return TYPE_EXIT;
        default:
            std::cerr << "readCommand: Unknown command: " << (int)buffer[0] << std::endl;
            DWORD written;
            WriteFile(hPipe, ERROR_UNKNOWN_COMMAND, 1, &written, NULL);  // Отправляем 1 байт
            return nullptr;
    }
}

void ServerController::clientThread(unsigned int id) {
    HANDLE hPipe = createAndConnectPipe(id);
    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Client thread " << id << " failed to create pipe\n";
        return;
    }

    std::cout << "Connection with client " << id << " is successful!\n";

    while (true) {
        const char* command = readCommand(hPipe);
        if (command == nullptr) {
            std::cerr << "Client " << id << " disconnected or error reading command\n";
            break;
        }

        if (command == TYPE_EXIT) {
            std::cout << "Client " << id << " requested exit\n";
            break;
        }

        unsigned int recordId;
        if (!checkIfValid(hPipe, recordId)) {
            std::cerr << "Client " << id << " sent invalid record ID\n";
            continue;
        }
        unsigned int indexRecord = fileController.getIndex(recordId);
        std::cout << "Client " << id << " processing record " << recordId << "\n";

        if (command == TYPE_MODIFY) {
            bool success = writerLogic(hPipe, recordId);
            std::cout << "Client " << id << " modify operation " << (success ? "succeeded" : "failed") << "\n";
        }
        else if (command == TYPE_READ) {
            bool success = readerLogic(hPipe, recordId);
            std::cout << "Client " << id << " read operation " << (success ? "succeeded" : "failed") << "\n";
        }
    }

    std::cout << "Closing connection with client " << id << "\n";
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
}

bool ServerController::checkIfValid(HANDLE hPipe, unsigned int &recordId) {
    const char* error;
    DWORD bytesWritten, bytesRead;

    std::cout << "DEBUG: Reading recordId from client..." << std::endl;
    if (!ReadFile(hPipe, &recordId, sizeof(recordId), &bytesRead, NULL) || bytesRead != sizeof(recordId)) {
        std::cerr << "checkIfValid: ReadFile failed or incomplete read. Bytes read: " << bytesRead << std::endl;
        return false;
    }
    std::cout << "DEBUG: Received recordId: " << recordId << std::endl;

    bool isFound = fileController.isThereId(recordId);
    std::cout << "DEBUG: Record found: " << (isFound ? "YES" : "NO") << std::endl;

    if (!isFound) {
        error = ERROR_ID_NOT_FOUND;
    } else {
        error = ERROR_OK;
    }

    std::cout << "DEBUG: Sending error code: '" << error << "'" << std::endl;

    // КЛЮЧЕВОЕ ИСПРАВЛЕНИЕ: всегда отправляем ровно 1 байт (первый символ строки)
    BOOL result = WriteFile(hPipe, error, 1, &bytesWritten, NULL);
    if (!result) {
        std::cerr << "checkIfValid: WriteFile failed: " << GetLastError() << std::endl;
        return false;
    }
    std::cout << "DEBUG: Sent " << bytesWritten << " bytes to client" << std::endl;

    return isFound;
}

bool ServerController::writerLogic(HANDLE hPipe, unsigned int recordId) {
    Employee emp;
    DWORD bytesRead, bytesWritten;
    unsigned int indexRecord = fileController.getIndex(recordId);

    if (indexRecord == (unsigned int)-1) {
        std::cerr << "Invalid index for record ID " << recordId << std::endl;
        return false;
    }

    // Блокируем запись
    recordLocks[indexRecord].lockForWrite();

    // Выполняем чтение
    bool okRead = fileController.readRecordEmp(recordId, emp);
    if (!okRead) {
        std::cerr << "DEBUG: Failed to read record from file" << std::endl;
        recordLocks[indexRecord].unlockFromRead();
        return false;
    }

    std::cout << "DEBUG: Read employee data: " << print(emp) << std::endl;

    // Отправляем данные клиенту
    std::cout << "DEBUG: Sending employee data to client (size: " << sizeof(emp) << ")..." << std::endl;
    if (!WriteFile(hPipe, &emp, sizeof(emp), &bytesWritten, NULL)) {
        std::cerr << "readerLogic: Failed to send employee data. Error: " << GetLastError() << std::endl;
        recordLocks[indexRecord].unlockFromRead();
        return false;
    }
    std::cout << "DEBUG: Sent " << bytesWritten << " bytes" << std::endl;

    // Читаем данные Employee от клиента
    if (!ReadFile(hPipe, &emp, sizeof(emp), &bytesRead, NULL) || bytesRead != sizeof(emp)) {
        std::cerr << "writerLogic: Failed to read Employee data\n";
        return false;
    }


    std::cout << "Client locked record " << recordId << " for writing. Waiting for client confirmation to unlock...\n";

    // Выполняем модификацию
    bool okModify = fileController.modifyRecords(recordId, emp);
    const char* error;
    if (!okModify)
        error = ERROR_FAILED_MODIFY;
    else
        error = ERROR_OK;

    // Отправляем результат операции клиенту
    WriteFile(hPipe, error, strlen(error), &bytesWritten, NULL);

    // Ждем команду от клиента для освобождения блокировки
    char buffer[124];
    if (!ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL)) {
        std::cerr << "writerLogic: Failed to read unlock confirmation from client\n";
        recordLocks[indexRecord].unlockFromWrite();
        return false;
    }

    // Освобождаем блокировку после получения команды от клиента
    recordLocks[indexRecord].unlockFromWrite();
    std::cout << "Client unlocked record " << recordId << " from writing\n";

    return okModify;
}

bool ServerController::readerLogic(HANDLE hPipe, unsigned int recordId) {
    Employee emp;
    DWORD bytesWritten, bytesRead;
    unsigned int indexRecord = fileController.getIndex(recordId);

    std::cout << "DEBUG: Starting readerLogic for recordId: " << recordId << std::endl;

    if (indexRecord == (unsigned int)-1) {
        std::cerr << "Invalid index for record ID " << recordId << std::endl;
        return false;
    }

    // Блокируем чтение
    recordLocks[indexRecord].lockForRead();
    std::cout << "DEBUG: Locked record for reading" << std::endl;

    // Выполняем чтение
    bool okRead = fileController.readRecordEmp(recordId, emp);
    if (!okRead) {
        std::cerr << "DEBUG: Failed to read record from file" << std::endl;
        recordLocks[indexRecord].unlockFromRead();
        return false;
    }

    std::cout << "DEBUG: Read employee data: " << print(emp) << std::endl;

    // Отправляем данные клиенту
    std::cout << "DEBUG: Sending employee data to client (size: " << sizeof(emp) << ")..." << std::endl;
    if (!WriteFile(hPipe, &emp, sizeof(emp), &bytesWritten, NULL)) {
        std::cerr << "readerLogic: Failed to send employee data. Error: " << GetLastError() << std::endl;
        recordLocks[indexRecord].unlockFromRead();
        return false;
    }
    std::cout << "DEBUG: Sent " << bytesWritten << " bytes" << std::endl;

    // Ждем команду от клиента для освобождения блокировки
    std::cout << "DEBUG: Waiting for client confirmation..." << std::endl;
    char confirmation;
    if (!ReadFile(hPipe, &confirmation, 1, &bytesRead, NULL)) {
        std::cerr << "readerLogic: Failed to read unlock confirmation. Error: " << GetLastError() << std::endl;
        recordLocks[indexRecord].unlockFromRead();
        return false;
    }
    std::cout << "DEBUG: Received confirmation: '" << confirmation << "' (" << bytesRead << " bytes)" << std::endl;

    // Освобождаем блокировку
    recordLocks[indexRecord].unlockFromRead();
    std::cout << "DEBUG: Unlocked record from reading" << std::endl;

    return okRead;
}