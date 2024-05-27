//
// Created by Altezza on 05.05.2024.
//

#ifndef DATABASEPROJECT_GET_OPERATOR_HPP
#define DATABASEPROJECT_GET_OPERATOR_HPP


#include "../structs/Condition.hpp"

static auto get_operator(Condition &cond) {
    return query_operator_str[static_cast<int>(cond.get_operator())];
}


#endif //DATABASEPROJECT_GET_OPERATOR_HPP
