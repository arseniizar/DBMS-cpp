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

// I need to check whether this table exists
// I need to check whether the inserts are proper fields of the table

std::vector<column> executor::join() {
    return std::vector<column>();
}

std::vector<column> executor::left_join() {
    return std::vector<column>();
}

std::vector<column> executor::right_join() {
    return std::vector<column>();
}

std::vector<column> executor::full_join() {
    return std::vector<column>();
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

