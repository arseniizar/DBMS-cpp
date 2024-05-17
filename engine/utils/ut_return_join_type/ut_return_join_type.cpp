//
// Created by Altezza on 17.05.2024.
//

#include "ut_return_join_type.hpp"
#include "../ut_str_toupper/ut_str_toupper.hpp"
#include "../ut_find_in_vector/ut_find_in_vector.hpp"
#include <fmt/core.h>

std::string return_join_type_str(std::string const &join) {
    auto s = join;
    auto index = 0;
    str_toupper(s);
    for (auto const &j: joins)
        if (j == s) {
            auto pair = ut_find_in_vector(joins, j);
            index = pair.second;
        }
    if (index == -1) return *joins.rend();
    else return joins.at(index);
}

join_type return_join_type(std::string const &join) {
    if (join == "JOIN") return join_type::JOIN;
    else if (join == "LEFT JOIN") return join_type::LEFT_JOIN;
    else if (join == "RIGHT JOIN") return join_type::RIGHT_JOIN;
    else if (join == "FULL JOIN") return join_type::FULL_JOIN;
    else return join_type::UNKNOWN;
}