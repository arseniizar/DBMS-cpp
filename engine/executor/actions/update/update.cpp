//
// Created by Altezza on 18.05.2024.
//

#include <fmt/core.h>
#include "../../Executor.hpp"

std::vector<Column> Executor::update() {
    auto vec = std::vector<Column>();
    Executor::action = Q_action::UPDATE;

    for (const auto updates = Executor::q.get_updates();
         const auto& [field_to_update, new_value, d_t] : updates) {
        Column col;
        col.set_name(field_to_update.value);
        col.set_type(d_t);
        col.add_row(Row(new_value, d_t));
        vec.push_back(col);
    }

    return vec;
}
