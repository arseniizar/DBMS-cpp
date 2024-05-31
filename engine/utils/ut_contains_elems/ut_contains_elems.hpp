//
// Created by Altezza on 31.05.2024.
//

#ifndef DATABASEPROJECT_UT_CONTAINS_ELEMS_HPP
#define DATABASEPROJECT_UT_CONTAINS_ELEMS_HPP

#include <vector>
#include <unordered_set>

// whether vec2 is a sub vec to vec1
template <typename T>
auto ut_contains_elems(std::vector<T> const& vec1, std::vector<T> const& vec2) -> bool {
    auto un_set = std::unordered_set<T>(vec1.begin(), vec1.end());
    for(auto const& el : vec2)
        if(un_set.find(el) == un_set.end())
            return false;
    return true;
};
// (c) ChatGPT

#endif //DATABASEPROJECT_UT_CONTAINS_ELEMS_HPP
