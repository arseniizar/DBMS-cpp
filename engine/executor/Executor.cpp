//
// Created by Altezza on 03.05.2024.
//
#include <bits/stdc++.h>

#include <utility>
#include "fmt/core.h"
#include "Executor.hpp"

Q_action Executor::get_action() const {
    return Executor::action;
}

void Executor::set_error(const Execution_error &err) {
    Executor::error = err;
}

Execution_error Executor::get_error() const {
    return Executor::error;
}

void Executor::set_query(const Query &que) {
    Executor::q = que;
}

Query Executor::get_query() const {
    return Executor::q;
}

Execution_result Executor::get_execution_res() const {
    return Executor::result;
}

// I need to check whether this Table exists
// I need to check whether the inserts are proper fields of the Table

std::vector<Column> Executor::join() {
    return std::vector<Column>();
}

std::vector<Column> Executor::left_join() {
    return std::vector<Column>();
}

std::vector<Column> Executor::right_join() {
    return std::vector<Column>();
}

std::vector<Column> Executor::full_join() {
    return std::vector<Column>();
}

void Executor::execute() {
    // depending on the type of the Query I will do something from above
    auto query = Executor::q;
    auto query_type = query_type_str[static_cast<int>(query.get_query_type())];
    if (query_type == "Select") {
        Executor::tmp_cols = Executor::select();
    } else if (query_type == "Update") {
        Executor::tmp_cols = Executor::update();
    } else if (query_type == "Insert") {
        Executor::tmp_cols = Executor::insert();
    } else if (query_type == "Delete") {
        // also could be the whole Table!
        Executor::tmp_cols = Executor::delete_f();
    } else if (query_type == "Create") {
        auto created_table = Executor::create_table();
        Executor::tmp_t = created_table;
    }
    Executor::result = Execution_result("Executed!", &query);
}

void Executor::set_tmp_t(Table t) {
    Executor::tmp_t = std::move(t);
}

