//
// Created by tvysh on 5/28/2025.
//

#include "ServerController.h"

ServerController::ServerController(unsigned int clients, unsigned int records, const char *filename,
                                   unsigned int capacity): numberOfClients(clients), numberOfRecords(records),
                                                           fileController(filename, capacity), recordLocks(records),
                                                           activeClientThreads(0), clientServerPipes(clients + 1){ // +1 чтобы индекс совпадал с id
    noActiveClientThreads = CreateEventA(NULL, FALSE, TRUE, NULL);
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
            0,                      // флаги создания
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
    startClientProcess(id);

    HANDLE hPipe = CreateNamedPipeA(pipeName.c_str(),
                                    PIPE_ACCESS_DUPLEX,
                                    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                                    1, 1024, 1024, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateNamedPipe " << id << " failed. Error: " << GetLastError() << "\n";
        return INVALID_HANDLE_VALUE;
    }

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

    if (!ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) || bytesRead == 0) {
        std::cerr << "readCommand: Failed to read or client disconnected\n";
        return nullptr;
    }

    if (bytesRead != 1) {
        std::cerr << "readCommand: Invalid command length: " << bytesRead << "\n";
        sendError(hPipe, ERROR_UNKNOWN_COMMAND);
        return nullptr;
    }

    sendError(hPipe, ERROR_OK);
    buffer[bytesRead] = '\0';

    switch (buffer[0]) {
        case TYPE_MODIFY[0]:
            std::cout << "Command received: MODIFY\n";
            return TYPE_MODIFY;
        case TYPE_READ[0]:
            std::cout << "Command received: READ\n";
            return TYPE_READ;
        case TYPE_EXIT[0]:
            std::cout << "Command received: EXIT\n";
            return TYPE_EXIT;
        default:
            std::cerr << "readCommand: Unknown command: " << buffer[0] << "\n";
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
    if (!ReadFile(hPipe, &recordId, sizeof(recordId), &bytesRead, NULL) || bytesRead != sizeof(recordId)) {
        std::cerr << "checkIfValid: ReadFile failed or incomplete read.\n";
        return false;
    }
    bool isFound = fileController.isThereId(recordId);
    if (!isFound)
        error = ERROR_ID_NOT_FOUND;
    else
        error = ERROR_OK;

    BOOL result = WriteFile(hPipe, error, strlen(error), &bytesWritten, NULL);
    if (!result) {
        std::cerr << "checkIfValid: WriteFile failed: " << GetLastError() << "\n";
        return false;
    }
    if (error != ERROR_OK)
        return false;
    return true;
}

bool ServerController::writerLogic(HANDLE hPipe, unsigned int recordId) {
    Employee emp;
    DWORD bytesRead, bytesWritten;

    // Читаем данные Employee от клиента
    if (!ReadFile(hPipe, &emp, sizeof(emp), &bytesRead, NULL) || bytesRead != sizeof(emp)) {
        std::cerr << "writerLogic: Failed to read Employee data\n";
        return false;
    }

    // Блокируем запись
    recordLocks[recordId].lockForWrite();
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
        recordLocks[recordId].unlockFromWrite();
        return false;
    }

    // Освобождаем блокировку после получения команды от клиента
    recordLocks[recordId].unlockFromWrite();
    std::cout << "Client unlocked record " << recordId << " from writing\n";

    return okModify;
}

bool ServerController::readerLogic(HANDLE hPipe, unsigned int recordId) {
    Employee emp;
    DWORD bytesWritten, bytesRead;

    // Блокируем чтение
    recordLocks[recordId].lockForRead();
    std::cout << "Client locked record " << recordId << " for reading. Waiting for client confirmation to unlock...\n";

    // Выполняем чтение
    bool okRead = fileController.readRecordEmp(recordId, emp);
    const char* error;
    if (!okRead)
        error = ERROR_FAILED_READ;
    else
        error = ERROR_OK;

    // Отправляем результат операции
    WriteFile(hPipe, error, strlen(error), &bytesWritten, NULL);
    if (error == ERROR_OK) {
        WriteFile(hPipe, &emp, sizeof(emp), &bytesWritten, NULL);
    }

    // Ждем команду от клиента для освобождения блокировки
    char buffer[124];
    if (!ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL)) {
        std::cerr << "readerLogic: Failed to read unlock confirmation from client\n";
        recordLocks[recordId].unlockFromRead();
        return false;
    }

    // Освобождаем блокировку после получения команды от клиента
    recordLocks[recordId].unlockFromRead();
    std::cout << "Client unlocked record " << recordId << " from reading\n";

    return okRead;
}