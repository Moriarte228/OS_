//
// Created by tvysh on 5/28/2025.
//

#ifndef OS_SERVERCONTROLLER_H
#define OS_SERVERCONTROLLER_H

#include "FileController.h"
#include "RecordLock.h"

class ServerController {
private:
    unsigned int numberOfClients;
    unsigned int numberOfRecords;
    std::vector<RecordLock> recordLocks;
    FileController fileController;
    std::atomic<int> activeClientThreads;
    HANDLE noActiveClientThreads;
    std::vector<HANDLE> clientServerPipes;
    void startClientProcess(unsigned int id);
    HANDLE createAndConnectPipe(unsigned int id);
    void sendError(HANDLE hPipe, const char* error);
    const char* readCommand(HANDLE hPipe);
public:
    ServerController(unsigned int clients, unsigned int records, const char* file,
                     unsigned int capacity);
    ~ServerController();
    bool addRecord(Employee emp);
    bool createThreads();
    bool readerLogic(HANDLE hPipe, unsigned int recordId);
    bool writerLogic(HANDLE hPipe, unsigned int recordId);
    bool checkIfValid(HANDLE hPipe, unsigned int& recordId);
    void clientThread(unsigned int id);
    std::string printFile(){
        return fileController.toString();
    }
    void waitForClientsThreadsEnd() {
        WaitForSingleObject(noActiveClientThreads, INFINITE);
    }
};


#endif //OS_SERVERCONTROLLER_H
