//
// Created by Altezza on 18.05.2024.
//

#include <fmt/core.h>
#include "../../Executor.hpp"

std::vector<Column> Executor::select() {
    auto fields = Executor::q.get_fields();
    auto query_table = *(q.get_p_table());
    auto vec = std::vector<Column>();
    if (fields.begin()->value == "*") {
        return query_table.get_columns();
    } else {
        for (const auto& field: fields)
            for (auto &column: query_table.get_columns())
                if (column.get_name() == field.value) vec.push_back(column);
    }
    Executor::action = Q_action::SELECT;
    return vec;
}