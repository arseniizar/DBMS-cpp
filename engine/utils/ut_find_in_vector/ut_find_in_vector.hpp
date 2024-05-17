//
// Created by Altezza on 17.05.2024.
//

#ifndef DATABASEPROJECT_UT_FIND_IN_VECTOR_HPP
#define DATABASEPROJECT_UT_FIND_IN_VECTOR_HPP

#include <vector>
#include <algorithm>

template<typename T>
std::pair<bool, int> ut_find_in_vector(std::vector<T> const &vec, T const &element) {
    using namespace std;
    auto res = std::pair<bool, int>();
    auto it = std::ranges::find(vec.begin(), vec.end(), element);
    if (it != vec.end()) {
        res.first = true;
        res.second = std::distance(vec.begin(), it);
    } else {
        res.first = false;
        res.second = -1;
    }
    return res;
}

#endif //DATABASEPROJECT_UT_FIND_IN_VECTOR_HPP
