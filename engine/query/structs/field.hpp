//
// Created by Altezza on 19.05.2024.
//

#ifndef DATABASEPROJECT_FIELD_HPP
#define DATABASEPROJECT_FIELD_HPP


#include <string>
#include "../../table/structs/data_type.hpp"
#include "key_attr.hpp"

struct field {
    std::string value;
    data_type d_t;
    key_attr k_a;
};


#endif //DATABASEPROJECT_FIELD_HPP
