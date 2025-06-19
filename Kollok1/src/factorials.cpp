// factorials.cpp
#include "factorials.h"

std::vector<unsigned long long> generate_factorials(unsigned int n) {
    if (n == 0)
        throw std::invalid_argument("n must be a positive integer");

    std::vector<unsigned long long> result;
    result.reserve(n);
    unsigned long long fact = 1;
    for (unsigned int i = 1; i <= n; ++i) {
        fact *= i;
        result.push_back(fact);
    }
    return result;
}
