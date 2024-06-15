//
// Created by Altezza on 31.05.2024.
//

#ifndef DATABASEPROJECT_UT_CONTAINS_ELEMS_HPP
#define DATABASEPROJECT_UT_CONTAINS_ELEMS_HPP

#include <vector>
#include <unordered_set>

template <typename T>
auto ut_contains_elems(std::vector<T> const& vec, std::vector<T> const& subvec) -> bool {
    auto un_set = std::unordered_set<T>(vec.begin(), vec.end());
    for(auto const& el : subvec)
        if(un_set.find(el) == un_set.end())
            return false;
    return true;
};
// (c) ChatGPT

#endif //DATABASEPROJECT_UT_CONTAINS_ELEMS_HPP
