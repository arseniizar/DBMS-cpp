#include "../Dbms.hpp"
#include "../../utils/ut_contains_elems/ut_contains_elems.hpp"

std::pair<std::vector<Column>, Execution_error> Dbms::execute_query(Query &q) {
    Dbms::executor.set_query(q);
    Dbms::executor.execute();
    Execution_result rec = Dbms::executor.get_execution_res();
    auto pair = std::pair<std::vector<Column>, Execution_error>();
    switch (Dbms::executor.action) {
        case Q_action::SELECT: {
            pair = Dbms::execute_select();
            goto def;
        }
        case Q_action::INSERT: {
            pair = Dbms::execute_insert();
            goto def;
        }
        case Q_action::DELETE: {
            pair = Dbms::execute_delete_from();
            goto def;
        }
        case Q_action::CREATE: {
            pair = Dbms::execute_create_table();
            if (!pair.second.message.empty()) {
                fmt::println("{}", pair.second.message);
                return make_executor_error(pair.second.message);
            }
            goto def;
        }
        case Q_action::DROP: {
            pair = Dbms::drop_table(Dbms::executor.q.get_table_name());
            if (!pair.second.message.empty()) {
                fmt::println("{}", pair.second.message);
                return make_executor_error(pair.second.message);
            }
            return make_executor_error("");
        }
        def:
        default: {
            std::string table_name = Dbms::executor.tmp_t.get_table_name();
            Table table = Dbms::find_table_by_name(Dbms::executor.tmp_t.get_table_name());
            // checking whether query cols names are the same as the cols of table
            auto are_cols_consistent = ut_contains_elems(
                    table.get_columns_names(),
                    Table("func", pair.first).get_columns_names()
            );
            if (!are_cols_consistent
                and Dbms::executor.q.get_query_type() != Query_type::Select) {
                Dbms::executor.error = Execution_error("at EXECUTION: provided fields do not exist in the table");
            }
            q.set_p_table(&table);
            auto rec_c = recent_change(&q);
            Dbms::add_rec_change(rec_c);
            Dbms::queries.push_back(q);
            if (!Dbms::executor.get_error().message.empty())
                fmt::println("{}", Dbms::executor.get_error().message);
            else fmt::println("EXECUTED SUCCESSFULLY");
            return std::make_pair(Dbms::executor.tmp_cols, Dbms::executor.error);
        }
    }
}

std::pair<std::vector<Column>, Execution_error> Dbms::execute_create_table() {
    auto table_name = Dbms::executor.tmp_t.get_table_name();
    if (Dbms::executor.q.get_fields().empty()) {
        return make_executor_error("at EXECUTION: you need at least one column to create a table");
    }
    if (is_table_already_exist(table_name)) {
        return make_executor_error("at EXECUTION: Table with name already exists");
    }
    if (!Dbms::executor.q.get_referenced_table_name().empty()) {
        auto are_relations_ok = Dbms::check_relations();
        if (are_relations_ok) Dbms::populate_keys();
    }
    Dbms::add_table(Dbms::executor.tmp_t);
    if (!Dbms::is_loading) ut_print(Dbms::executor.tmp_t);
    return std::make_pair(Dbms::executor.tmp_t.get_columns(), Execution_error());
}

std::pair<std::vector<Column>, Execution_error> Dbms::execute_select() {
    auto table_name = Dbms::executor.q.get_table_name();
    auto conditions = Dbms::executor.q.get_conditions();
    if (!is_table_already_exist(table_name)) {
        std::string message = "at EXECUTION: unable to select columns from nonexistent table";
        Dbms::executor.error = Execution_error(message);
        return make_executor_error(message);
    }
    auto table = Dbms::find_table_by_name(table_name);
    auto are_consistent_error = Dbms::are_consistent().second;
    if (!are_consistent_error.message.empty()) {
        return make_executor_error(are_consistent_error.message);
    }
    if (!conditions.empty()) {
        auto cols = table.find_cols_by_conditions(conditions);
        ut_print(cols);
        return make_executor_error("");
    } else {
        if (Dbms::executor.q.get_fields().at(0).value == "*") {
            ut_print(table.get_columns());
            return make_executor_error("");
        }
        auto selected_cols = std::vector<Column>();
        for (auto &field: Dbms::executor.q.get_fields()) {
            for (auto &col: table.get_columns()) {
                if (col.get_name() == field.value) {
                    selected_cols.push_back(col);
                }
            }
        }
        ut_print(selected_cols);
        return make_executor_error("");
    }
}

std::pair<std::vector<Column>, Execution_error> Dbms::execute_delete_from() {
    auto table_name = Dbms::executor.q.get_table_name();
    auto table = Dbms::find_table_by_name(table_name);
    auto conditions = Dbms::executor.q.get_conditions();
    if (!is_table_already_exist(table_name)) {
        std::string message = "at EXECUTION: unable to delete from nonexistent table";
        Dbms::executor.error = Execution_error(message);
        return make_executor_error(message);
    }
    auto are_consistent_error = Dbms::are_consistent().second;
    if (!are_consistent_error.message.empty()) {
        return make_executor_error(are_consistent_error.message);
    }
    Execution_error exe_er = Dbms::add_and_override_cols(table_name,
                                                         table.find_cols_by_conditions(
                                                                 Dbms::executor.q.get_conditions()));
    if (!exe_er.message.empty()) {
        Dbms::executor.error = exe_er;
        return make_executor_error(exe_er.message);
    }
    return std::make_pair(Dbms::executor.tmp_cols, Execution_error());
}

std::pair<std::vector<Column>, Execution_error> Dbms::execute_insert() {
    auto table_name = Dbms::executor.q.get_table_name();
    if (!is_table_already_exist(table_name)) {
        std::string message = "at EXECUTION: unable to insert to nonexistent table";
        Dbms::executor.error = Execution_error(message);
        return make_executor_error(message);
    }
    auto are_consistent_error = Dbms::are_consistent().second;
    if (!are_consistent_error.message.empty()) {
        return make_executor_error(are_consistent_error.message);
    }
    Execution_error exe_er = Dbms::add_and_override_cols(table_name, Dbms::executor.tmp_cols);
    if (!exe_er.message.empty()) {
        Dbms::executor.error = exe_er;
        return make_executor_error(exe_er.message);
    }
    return std::make_pair(Dbms::executor.tmp_cols, Execution_error());
}

std::pair<std::vector<Column>, Execution_error> Dbms::drop_table(std::string const &table_name) {
    if (!is_table_already_exist(table_name)) {
        return make_executor_error("at EXECUTION: unable to drop a nonexistent table");
    }
    auto it = std::find_if(Dbms::tables.begin(), Dbms::tables.end(),
                           [&table_name](Table &t) {
                               return t.get_table_name() == table_name;
                           });
    Dbms::tables.erase(it);
    fmt::println("dropped table: {}", table_name);
    return make_executor_error("");
}
