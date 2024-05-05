//
// Created by Altezza on 05.05.2024.
//

#ifndef DATABASEPROJECT_QUERY_OPERATOR_H
#define DATABASEPROJECT_QUERY_OPERATOR_H


#include <string>

enum struct query_operator {
    UnknownOperator,
    // =
    eq,
    // !=
    ne,
    // >
    gt,
    // <
    lt,
    // >=
    gte,
    // <=
    lte
};

inline std::string query_operator_str[] = {
        "unknown_operator",
        "eq",
        "ne",
        "gt",
        "lt",
        "gte",
        "lte",
};


#endif //DATABASEPROJECT_QUERY_OPERATOR_H
