//
// Created by Altezza on 05.05.2024.
//

#ifndef DATABASEPROJECT_CONDITION_H
#define DATABASEPROJECT_CONDITION_H


#include <string>
#include "query_operator.h"

struct condition {
    std::string operand1;
    bool operand1_is_field;
    query_operator query_operator;
    std::string operand2;
    bool operand2_is_field;
};

#endif //DATABASEPROJECT_CONDITION_H
