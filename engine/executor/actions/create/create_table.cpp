//
// Created by Altezza on 18.05.2024.
//

#include <fmt/core.h>
#include "../../executor.hpp"

table executor::create_table() {
    executor::action = q_action::CREATE;
    table created_table = table();
    created_table.set_table_name(executor::q.get_table_name());
    for (auto const& field: executor::q.get_fields()) {
        column col = column(std::vector<row>(), field.value, field.d_t);
        created_table.insert_column(col);
    }
    return created_table;
}