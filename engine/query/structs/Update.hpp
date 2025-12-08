//
// Created by Altezza on 07.06.2024.
//

#ifndef DATABASEPROJECT_UPDATE_HPP
#define DATABASEPROJECT_UPDATE_HPP


#include <string>
#include "../../table/structs/Data_type.hpp"

struct Update {
    Field field_to_update;
    std::string new_value;
    Data_type d_t;
};


#endif //DATABASEPROJECT_UPDATE_HPP
