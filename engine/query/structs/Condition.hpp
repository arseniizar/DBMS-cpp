//
// Created by Altezza on 05.05.2024.
//

#ifndef DATABASEPROJECT_CONDITION_HPP
#define DATABASEPROJECT_CONDITION_HPP


#include <string>
#include "Query_operator.hpp"
#include "fmt/core.h"
#include "fmt/ranges.h"

enum struct Condition_type {
    WHERE,
    HAVING
};

struct Condition {
public:
    std::string operand1;
    bool operand1_is_field;
    Query_operator _operator = Query_operator::UnknownOperator;
    std::string operand2;
    bool operand2_is_field;
    Data_type d_t = Data_type::UNKNOWN;
    Condition_type c_type = Condition_type::WHERE;

    Condition(std::string const &identifier, bool b) {
        Condition::operand1 = identifier;
        Condition::operand1_is_field = b;
    }

    Condition(std::string const &identifier, bool b, Data_type t) {
        Condition::operand1 = identifier;
        Condition::operand1_is_field = b;
        Condition::d_t = t;
    }

    Condition(std::string const &identifier, bool b, Condition_type type) {
        Condition::operand1 = identifier;
        Condition::operand1_is_field = b;
        Condition::c_type = type;
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

    auto get_field() {
        if(Condition::operand1_is_field) return Condition::operand1;
        if(Condition::operand2_is_field) return Condition::operand2;
        return std::string("ERROR!");
    }
};

#endif //DATABASEPROJECT_CONDITION_HPP
