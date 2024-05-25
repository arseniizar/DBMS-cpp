//
// Created by Altezza on 18.05.2024.
//

#include <fmt/core.h>
#include "../../executor.hpp"



std::vector<column> executor::delete_f() {
    auto conditions = executor::q.get_conditions();
    auto query_table_pointer = q.get_table_pointer();
    // if the conditions vector is empty then it means that it deletes the whole table
    if (conditions.empty()) {
        // delete every field from the table, additionally put it in the backup
        auto rec_change = query_table_pointer->erase();
        executor::action = q_action::DELETE;
        return rec_change;
    } else {
        auto rec_change = query_table_pointer->erase(conditions);
        executor::action = q_action::DELETE;
        return rec_change;
    }
}