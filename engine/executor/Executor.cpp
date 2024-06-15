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
        Executor::tmp_t = Executor::create_table();
    }
    Executor::result = Execution_result("Executed!", &query);
}

void Executor::set_tmp_t(Table t) {
    Executor::tmp_t = std::move(t);
}

void Executor::clean_error() {
    Executor::error = Execution_error();
}

