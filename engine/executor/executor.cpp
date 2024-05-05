//
// Created by Altezza on 03.05.2024.
//
#include <bits/stdc++.h>
#include "fmt/core.h"
#include "executor.h"

auto executor::set_query(struct query &q) {
    executor::query = q;
}

struct table executor::select() {
    auto fields = executor::query.get_fields();
    auto query_table = *(query.get_table_pointer());
    auto vec = std::vector<column>();
    if (fields.at(0) == "*") {
        return query_table;
    } else {
        for (auto const &field: fields)
            for (auto &column: query_table.get_columns())
                if (column.get_name() == field) vec.push_back(column);
    }
    return table{query_table.get_table_name(), vec};
}

auto executor::delete_f() {
    auto conditions = executor::query.get_conditions();
    auto query_table_pointer = query.get_table_pointer();
    // if the conditions vector is empty then it means that it deletes the whole table
    if(conditions.empty()) {
        // delete every field from the table, additionally put it in the backup
        query_table_pointer->erase();
    } else {
        query_table_pointer->erase(conditions);
    }
}

auto executor::insert() {
    return nullptr;
}

auto executor::update() {
    return nullptr;
}

void executor::execute() {
    // depending on the type of the query I will do something from above
}