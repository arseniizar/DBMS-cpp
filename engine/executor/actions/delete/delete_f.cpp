//
// Created by Altezza on 18.05.2024.
//

#include <fmt/core.h>
#include "../../Executor.hpp"

std::vector<Column> Executor::delete_f() {
    auto conditions = Executor::q.get_conditions();
    auto query_table_pointer = q.get_p_table();
    // if the conditions vector is empty then it means that it deletes the whole Table
    if (conditions.empty()) {
        // delete every Field from the Table, additionally put it in the backup
        auto rec_change = query_table_pointer->erase();
        Executor::action = Q_action::DELETE;
        return rec_change;
    } else {
        auto rec_change = query_table_pointer->erase(conditions);
        Executor::action = Q_action::DELETE;
        return rec_change;
    }
}