//
// Created by Altezza on 03.05.2024.
//
#include <bits/stdc++.h>
#include "fmt/core.h"
#include "executor.h"
#include "../query/query.h"

void executor::set_query(const query &q) {
    executor::q = q;
}

query executor::get_query() {
    return executor::q;
}

auto executor::select() {
    auto fields = executor::q.get_fields();
    auto query_table = *(q.get_table_pointer());
    auto vec = std::vector<column>();
    if (fields.begin()->first == "*") {
        return query_table.get_columns();
    } else {
        for (auto [col_name, _] : fields)
            for (auto &column: query_table.get_columns())
                if (column.get_name() == col_name) vec.push_back(column);
    }
    return vec;
}

auto executor::delete_f() {
    auto conditions = executor::q.get_conditions();
    auto query_table_pointer = q.get_table_pointer();
    // if the conditions vector is empty then it means that it deletes the whole table
    if (conditions.empty()) {
        // delete every field from the table, additionally put it in the backup
        auto rec_change = query_table_pointer->erase();
        return rec_change;
    } else {
        auto rec_change = query_table_pointer->erase(conditions);
        return rec_change;
    }
}

auto executor::insert() {
    auto vec = std::vector<column>();
    return vec;
}

auto executor::update() {
    auto vec = std::vector<column>();
    return vec;
}

std::vector<column> executor::execute() {
    // depending on the type of the q I will do something from above
    auto query = executor::q;
    auto query_type = query_type_str[static_cast<int>(query.get_type())];
    auto res = std::vector<column>();
    if(query_type == "Select") {
        res = executor::select();
    } else if (query_type == "Update") {
        res = executor::update();
    } else if (query_type == "Insert") {
        res = executor::insert();
    } else if (query_type == "Delete") {
        res = executor::delete_f();
    }
    executor::result = execution_result("Executed!", &query);
    return res;
}

