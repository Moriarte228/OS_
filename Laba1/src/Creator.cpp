
#include "employee.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: creator <binary_file> <num_entries>" << std::endl;
        return 1;
    }

    std::ofstream outFile(argv[1], std::ios::binary);
    if (!outFile) {
        std::cerr << "Cannot open file!" << std::endl;
        return 1;
    }

    int numEntries = std::stoi(argv[2]);  // Теперь std::stoi будет распознан
    employee emp;

    for (int i = 0; i < numEntries; ++i) {
        std::cout << "Enter employee number: ";
        std::cin >> emp.num;
        std::cout << "Enter employee name: ";
        std::cin >> emp.name;
        std::cout << "Enter hours worked: ";
        std::cin >> emp.hours;

        outFile.write(reinterpret_cast<char*>(&emp), sizeof(employee));
    }

    outFile.close();
    return 0;
}