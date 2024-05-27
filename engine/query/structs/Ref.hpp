//
// Created by Altezza on 19.05.2024.
//

#ifndef DATABASEPROJECT_REF_HPP
#define DATABASEPROJECT_REF_HPP

#include <string>
#include "../../table/structs/column/Column.hpp"

struct Ref {
    std::string table_name;
    std::string column_name;

    [[nodiscard]] bool empty() const {
        return column_name.empty() && table_name.empty();
    }
};

#endif //DATABASEPROJECT_REF_HPP
