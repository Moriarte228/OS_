//
// Created by tvysh on 5/28/2025.
//

#ifndef OS_FILECONTROLLER_H
#define OS_FILECONTROLLER_H

#include "Employee.h"

class FileController {
private:
    bool isCorrectHour(double hour) { return hour >= 0; };

    void createFileInterface(const char* filename);
    unsigned int capacity;
    unsigned int numOfRecords;
    unsigned int totalSize;
    std::map<unsigned int, unsigned int> idIndexMap;
    bool isOpen;
    HANDLE hFile;
    HANDLE hMap;
    char* pView;
    Employee* employee;
public:
    FileController(const char* filename, unsigned int capacity);
    ~FileController();

    bool isOpened(){ return isOpen; }
    unsigned int getNumOfRecords() { return numOfRecords; }
    bool isThereId(unsigned int id) { return idIndexMap.contains(id); };
    unsigned int getIndex(unsigned int id) { return isThereId(id)? idIndexMap[id]: -1; }

    bool addRecord(Employee emp);
    bool modifyName(unsigned int id, char name[10]);
    bool modifyHours(unsigned int id, double hours);
    bool readRecord(unsigned int id, std::string& result);
    bool readRecordEmp(unsigned int id, Employee& result);
    bool printFile(std::string& result);
    bool modifyRecords(unsigned int id, Employee emp);
};


#endif //OS_FILECONTROLLER_H
