//
// Created by Altezza on 05.05.2024.
//

#ifndef DATABASEPROJECT_QUERY_OPERATOR_H
#define DATABASEPROJECT_QUERY_OPERATOR_H


#include <string>
#include "../table/row.h"
#include "../table/data_type.h"

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

static bool predicate(std::string const &op, row row, std::string const &field) {
        if (op == "eq") {
            return (row.get_data() == field) && (row.get_type()
                                                 == return_data_type(field));
        } else if (op == "ne") {
            return (row.get_data() != field) && (row.get_type()
                                                 == return_data_type(field));
        } else if (op == "gt") {
            return (row.get_data() > field) && (row.get_type()
                                                == return_data_type(field));
        } else if (op == "lt") {
            return (row.get_data() < field) && (row.get_type()
                                                == return_data_type(field));
        } else if (op == "gte") {
            return (row.get_data() >= field) && (row.get_type()
                                                 == return_data_type(field));
        } else if (op == "lte") {
            return (row.get_data() <= field) && (row.get_type()
                                                 == return_data_type(field));
        }
    }


#endif //DATABASEPROJECT_QUERY_OPERATOR_H
