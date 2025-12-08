//
// Created by Altezza on 19.05.2024.
//

#ifndef DATABASEPROJECT_FIELD_HPP
#define DATABASEPROJECT_FIELD_HPP


#include <string>
#include "../../table/structs/Data_type.hpp"
#include "Key_attr.hpp"

enum struct Aggregation_type {
    NONE, COUNT
};

struct Field {
    std::string value;
    Data_type d_t;
    Key_attr k_a;
    Aggregation_type agg_t = Aggregation_type::NONE;

    [[nodiscard]] auto empty() const -> bool {
        return value.empty() && d_t == Data_type::UNKNOWN && k_a.empty();
    }
};


#endif //DATABASEPROJECT_FIELD_HPP
