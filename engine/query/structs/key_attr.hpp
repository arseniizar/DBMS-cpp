//
// Created by Altezza on 19.05.2024.
//

#ifndef DATABASEPROJECT_KEY_ATTR_HPP
#define DATABASEPROJECT_KEY_ATTR_HPP

#include "../../table/table.hpp"
#include "reference.hpp"

enum struct key_type {
    PK, FK, NONE,
};

struct key_attr {
    key_type k_t = key_type::NONE;
    reference reference;

    bool empty() {
        return reference.empty() && k_t == key_type::NONE;
    }
};

#endif //DATABASEPROJECT_KEY_ATTR_HPP
