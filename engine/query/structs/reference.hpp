//
// Created by Altezza on 19.05.2024.
//

#ifndef DATABASEPROJECT_REFERENCE_HPP
#define DATABASEPROJECT_REFERENCE_HPP

#include "../../table/table.hpp"

struct reference {
    std::string table_name;
    table *reference;

    bool empty() {
        return reference->empty() && table_name.empty();
    }
};

#endif //DATABASEPROJECT_REFERENCE_HPP
