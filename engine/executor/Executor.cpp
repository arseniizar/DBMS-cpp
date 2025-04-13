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
    switch (auto query_type = query.get_query_type()) {
        case Query_type::Select: {
            Executor::tmp_cols = Executor::select();
            break;
        }
        case Query_type::Insert: {
           Executor::tmp_cols = Executor::insert();
           break;
        }
        case Query_type::Create: {
            Executor::tmp_t = Executor::create_table();
            break;
        }
        case Query_type::Drop: {
            Executor::drop();
            break;
        }
        case Query_type::Delete: {
            Executor::tmp_cols = Executor::delete_f();
            break;
        }
        default: {
            auto message = "error during execution: unknown query type";
            fmt::println("{}", message);
            Executor::error = Execution_error(message);
            return;
        }
    }
    Executor::result = Execution_result("Executed!", &query);
}

void Executor::set_tmp_t(Table t) {
    Executor::tmp_t = std::move(t);
}

void Executor::clean_error() {
    Executor::error = Execution_error();
}

void Executor::drop() {
    Executor::action = Q_action::DROP;
}

