//
// Created by Altezza on 03.05.2024.
//

#ifndef DATABASEPROJECT_EXECUTION_RESULT_HPP
#define DATABASEPROJECT_EXECUTION_RESULT_HPP

#include <string>
#include "../../query/Query.hpp"

struct Execution_result {
    std::string message;
    Query* p_query;
};

#endif //DATABASEPROJECT_EXECUTION_RESULT_HPP
