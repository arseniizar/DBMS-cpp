//
// Created by Altezza on 05.05.2024.
//

#ifndef DATABASEPROJECT_CONDITION_HPP
#define DATABASEPROJECT_CONDITION_HPP


#include <string>
#include "query_operator.hpp"
#include "fmt/core.h"
#include "fmt/ranges.h"

struct condition {
public:
    std::string operand1;
    bool operand1_is_field;
    query_operator query_operator;
    std::string operand2;
    bool operand2_is_field;

    condition(std::string const &identifier, bool b) {
        condition::operand1 = identifier;
        condition::operand1_is_field = b;
    }

    auto to_string() {
        return std::string("operand1: " + condition::operand1 + "query_operator: " +
                           query_operator_str[static_cast<int>(query_operator)] + "operand2: " +
                           condition::operand2);
    }

    auto get_operator() {
        return condition::query_operator;
    }

    auto get_operand1() {
        return condition::operand1;
    };

    auto get_operand2() {
        return condition::operand2;
    };
};

#endif //DATABASEPROJECT_CONDITION_HPP
