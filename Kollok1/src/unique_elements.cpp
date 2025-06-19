// unique_elements.cpp
#include "unique_elements.h"

std::vector<int> remove_duplicates(const std::vector<int>& input) {
    std::unordered_set<int> seen;
    std::vector<int> result;
    for (int value : input) {
        if (seen.insert(value).second)
            result.push_back(value);
    }
    return result;
}
