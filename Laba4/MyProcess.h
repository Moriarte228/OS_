// Created by tvysh on 5/13/2025.

#ifndef OS_MYPROCESS_H
#define OS_MYPROCESS_H

#include <windows.h>
#include <sstream>
#include <string>

class MyProcess {
private:
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    bool created;
    void closeHandles() {
        if (pi.hThread) {
            CloseHandle(pi.hThread);
            pi.hThread = NULL;
        }
        if (pi.hProcess) {
            CloseHandle(pi.hProcess);
            pi.hProcess = NULL;
        }
        created = false;
    }
    void cleanPiSi() {
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
    }
    MyProcess(const MyProcess&);
    MyProcess& operator=(const MyProcess&);
public:
    MyProcess() : created(false) {
        cleanPiSi();
    }

    // Создание процесса
    bool createA(const std::ostringstream& cmd, bool bInheritHandles) {
        if (created)
            return false;
        cleanPiSi();
        std::string command = cmd.str();
        if (command.empty())
            return false;
        command.push_back('\0');

        char* buffer = &command[0];

        BOOL success = CreateProcessA(
                NULL,
                buffer,
                NULL, NULL, bInheritHandles,
                CREATE_NEW_CONSOLE,
                NULL, NULL,
                &si, &pi
        );

        if (!success) {
            return false;
        }

        created = true;
        return true;
    }


    bool isCreated() const {
        return created;
    }


    void terminate() {
        if (created) {
            TerminateProcess(pi.hProcess, 0);
            WaitForSingleObject(pi.hProcess, INFINITE);
            closeHandles();
        }
    }

    bool waitExit(DWORD timeout = INFINITE) {
        if (created) {
            DWORD result = WaitForSingleObject(pi.hProcess, timeout);
            if (result == WAIT_OBJECT_0) {
                closeHandles();
                return true;
            }
        }
        return false;
    }

    ~MyProcess() {
        if (created) {
            DWORD exitCode;
            if (GetExitCodeProcess(pi.hProcess, &exitCode) && exitCode == STILL_ACTIVE)
                terminate();
            else {
                closeHandles();
            }
        }
    }
};

#endif // OS_MYPROCESS_H
