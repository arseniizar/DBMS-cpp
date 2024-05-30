//
// Created by Altezza on 30.05.2024.
//

#ifndef DATABASEPROJECT_INSERT_HPP
#define DATABASEPROJECT_INSERT_HPP


#include <string>
#include "../../table/structs/Data_type.hpp"

struct Insert {
    std::string column_to_insert;
    std::string value;
    Data_type d_t;
};


#endif //DATABASEPROJECT_INSERT_HPP
