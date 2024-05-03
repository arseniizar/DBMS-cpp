//
// Created by Altezza on 03.05.2024.
//

#ifndef DATABASEPROJECT_EXECUTION_RESULT_H
#define DATABASEPROJECT_EXECUTION_RESULT_H

#include <string>
#include "../query.h"

struct execution_result {
    std::string message;
    query* p_query;
};

#endif //DATABASEPROJECT_EXECUTION_RESULT_H
