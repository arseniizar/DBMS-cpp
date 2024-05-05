//
// Created by Altezza on 05.05.2024.
//

#ifndef DATABASEPROJECT_GET_OPERATOR_H
#define DATABASEPROJECT_GET_OPERATOR_H


#include "condition.h"

static auto get_operator(const condition &cond) {
    return query_operator_str[static_cast<int>(cond.query_operator)];
}


#endif //DATABASEPROJECT_GET_OPERATOR_H
