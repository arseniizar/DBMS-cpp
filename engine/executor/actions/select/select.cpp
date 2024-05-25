//
// Created by Altezza on 18.05.2024.
//

#include <fmt/core.h>
#include "../../executor.hpp"

std::vector<column> executor::select() {
    auto fields = executor::q.get_fields();
    auto query_table = *(q.get_table_pointer());
    auto vec = std::vector<column>();
    if (fields.begin()->value == "*") {
        return query_table.get_columns();
    } else {
        for (const auto& field: fields)
            for (auto &column: query_table.get_columns())
                if (column.get_name() == field.value) vec.push_back(column);
    }
    executor::action = q_action::SELECT;
    return vec;
}