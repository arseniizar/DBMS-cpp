//
// Created by Altezza on 03.05.2024.
//
#include <bits/stdc++.h>
#include "fmt/core.h"
#include "executor.hpp"
#include "../query/query.hpp"

q_action executor::get_action() const {
    return executor::action;
}

void executor::set_error(const execution_error &err) {
    executor::error = err;
}

execution_error executor::get_error() const {
    return executor::error;
}

void executor::set_query(const query &que) {
    executor::q = que;
}

query executor::get_query() const {
    return executor::q;
}

execution_result executor::get_execution_res() const {
    return executor::result;
}

table executor::create_table() {
    executor::action = q_action::CREATE;
    table created_table = table();
    created_table.set_table_name(executor::q.get_table_name());
    for(auto [name, type] : executor::q.get_fields()) {
        column col = column(std::vector<row>(), name, type);
        created_table.insert_column(col);
    }
    return created_table;
}

std::vector<column> executor::select() {
    auto fields = executor::q.get_fields();
    auto query_table = *(q.get_table_pointer());
    auto vec = std::vector<column>();
    if (fields.begin()->first == "*") {
        return query_table.get_columns();
    } else {
        for (auto [col_name, _]: fields)
            for (auto &column: query_table.get_columns())
                if (column.get_name() == col_name) vec.push_back(column);
    }
    executor::action = q_action::SELECT;
    return vec;
}

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

std::vector<column> executor::insert() {
    auto vec = std::vector<column>();
    executor::action = q_action::INSERT;
    return vec;
}

std::vector<column> executor::update() {
    auto vec = std::vector<column>();
    executor::action = q_action::UPDATE;
    return vec;
}

void executor::execute() {
    // depending on the type of the query I will do something from above
    auto query = executor::q;
    auto query_type = query_type_str[static_cast<int>(query.get_query_type())];
    if (query_type == "Select") {
        executor::tmp_cols = executor::select();
    } else if (query_type == "Update") {
        executor::tmp_cols = executor::update();
    } else if (query_type == "Insert") {
        executor::tmp_cols = executor::insert();
    } else if (query_type == "Delete") {
        // also could be the whole table!
        executor::tmp_cols = executor::delete_f();
    } else if (query_type == "Create") {
       executor::tmp_t = executor::create_table();
    }
    executor::result = execution_result("Executed!", &query);
}
