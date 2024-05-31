//
// Created by Altezza on 18.05.2024.
//

#include <fmt/core.h>
#include "../../Executor.hpp"

std::vector<Column> Executor::delete_f() {
    auto conditions = Executor::q.get_conditions();
    auto res = std::vector<Column>();
    Executor::action = Q_action::DELETE;
    // if the conditions vector is empty then it means that it deletes the whole Table
    if (conditions.empty()) {
        // delete every row from the Table, additionally put it in the backup
        return {};
    }
    return res;
}