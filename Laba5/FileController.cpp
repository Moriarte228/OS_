//
// Created by tvysh on 5/28/2025.
//

#include "FileController.h"

void FileController::createFileInterface(const char* filename) {
    hFile = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open file." << std::endl;
        exit(1);
    }

    SetFilePointer(hFile, totalSize, NULL, FILE_BEGIN);
    SetEndOfFile(hFile);
    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

    hMap = CreateFileMappingA(hFile, NULL, PAGE_READWRITE, 0,
                              totalSize, NULL);
    if (!hMap) {
        std::cerr << "Failed to create file mapping." << std::endl;
        exit(1);
    }

    pView = (char*)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0,
                                 0, totalSize);
    if (!pView) {
        std::cerr << "Failed to map view of file." << std::endl;
        exit(1);
    }
}

FileController::FileController(const char* filename, unsigned int cap) {
    isOpen = false;
    if (cap <= 0) {
        std::cerr << "Non positive capacity!";
        exit(1);
    }
    capacity = cap;
    totalSize = sizeof(Employee) * capacity;
    this->createFileInterface(filename);
    employee = (Employee*) pView;
    numOfRecords = 0;
    idIndexMap = std::map <unsigned int, unsigned int>();
    isOpen = true;
}

FileController::~FileController() {
    if (isOpen) {
        UnmapViewOfFile(pView);
        CloseHandle(hMap);
        CloseHandle(hFile);
    }
}

bool FileController::addRecord(Employee emp) {
    if (numOfRecords >= capacity) {
        return false;
    }
    if (idIndexMap.contains(emp.id)) {
        return false;
    }

    unsigned int index = numOfRecords;
    employee[index] = emp;
    idIndexMap.insert(std::make_pair(emp.id, index));
    numOfRecords++;
    return true;
}

bool FileController::modifyName(unsigned int id, char name[10]) {
    if (!idIndexMap.contains(id)) {
        return false;
    }
    unsigned int index = idIndexMap[id];
    strcpy(employee[index].name, name);
    return true;
}

bool FileController::modifyHours(unsigned int id, double hours) {
    if (!idIndexMap.contains(id)) {
        return false;
    }
    unsigned int index = idIndexMap[id];
    employee[index].hours = hours;
    return true;
}

bool FileController::readRecord(unsigned int id, std::string &result) {
    if (!idIndexMap.contains(id)) {
        return false;
    }
    unsigned int index = idIndexMap[id];
    result = print(employee[index]);
    return true;
}

bool FileController::readRecordEmp(unsigned int id, Employee &result) {
    if (!idIndexMap.contains(id)) {
        return false;
    }
    unsigned int index = idIndexMap[id];
    result = employee[index];
    return true;
}

bool FileController::printFile(std::string &result) {
    result.clear();
    if (numOfRecords == 0){
        return false;
    }
    for (int i = 0; i < numOfRecords; ++i) {
        result += print(employee[i]);
    }
    return true;
}

bool FileController::modifyRecords(unsigned int id, Employee emp) {
    if (!idIndexMap.contains(id)) {
        return false;
    }
    unsigned int index = idIndexMap[id];
    if (emp.id != employee[index].id)
        return false;
    employee[index] = emp;
    return true;
}