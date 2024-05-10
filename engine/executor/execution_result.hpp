//
// Created by Altezza on 03.05.2024.
//

#ifndef DATABASEPROJECT_EXECUTION_RESULT_HPP
#define DATABASEPROJECT_EXECUTION_RESULT_HPP

#include <string>
#include "../query/query.hpp"

struct execution_result {
    std::string message;
    query* p_query;
};

#endif //DATABASEPROJECT_EXECUTION_RESULT_HPP
