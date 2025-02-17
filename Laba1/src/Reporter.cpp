//
// Created by tvysh on 2/11/2025.
//
#include "Employee.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: reporter <binary_file> <report_file> <hourly_rate>" << std::endl;
        return 1;
    }

    std::ifstream inFile(argv[1], std::ios::binary);
    if (!inFile) {
        std::cerr << "Cannot open binary file!" << std::endl;
        return 1;
    }

    std::ofstream outFile(argv[2]);
    if (!outFile) {
        std::cerr << "Cannot create report file!" << std::endl;
        return 1;
    }

    double hourlyRate = std::stod(argv[3]);  // Теперь std::stod будет распознан
    employee emp;

    outFile << "Report for file \"" << argv[1] << "\"" << std::endl;
    outFile << "Employee Number, Name, Hours, Salary" << std::endl;

    while (inFile.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
        double salary = emp.hours * hourlyRate;
        outFile << emp.num << ", " << emp.name << ", " << emp.hours << ", " << salary << std::endl;
    }

    inFile.close();
    outFile.close();
    return 0;
}