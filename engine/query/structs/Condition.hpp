//
// Created by Altezza on 05.05.2024.
//

#ifndef DATABASEPROJECT_CONDITION_HPP
#define DATABASEPROJECT_CONDITION_HPP


#include <string>
#include "Query_operator.hpp"
#include "fmt/core.h"
#include "fmt/ranges.h"

struct Condition {
public:
    std::string operand1;
    bool operand1_is_field;
    Query_operator _operator;
    std::string operand2;
    bool operand2_is_field;

    Condition(std::string const &identifier, bool b) {
        Condition::operand1 = identifier;
        Condition::operand1_is_field = b;
    }

    auto to_string() {
        return std::string("operand1: " + Condition::operand1 + "_operator: " +
                           query_operator_str[static_cast<int>(_operator)] + "operand2: " +
                           Condition::operand2);
    }

    auto get_operator() {
        return Condition::_operator;
    }

    auto get_operand1() {
        return Condition::operand1;
    };

    auto get_operand2() {
        return Condition::operand2;
    };
};

#endif //DATABASEPROJECT_CONDITION_HPP
