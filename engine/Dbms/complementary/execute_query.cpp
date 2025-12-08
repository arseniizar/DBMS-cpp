#include <ranges>

#include "../Dbms.hpp"
#include "../../utils/ut_contains_elems/ut_contains_elems.hpp"
#include "../../utils/ut_print/ut_print.hpp"
#include <set>
#include <map>

std::pair<std::vector<Column>, Execution_error> Dbms::execute_query(Query& q) {
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

    std::vector<Column> result_cols;

    if (conditions.empty()) {
        if (Dbms::executor.q.get_fields().at(0).value == "*") {
            result_cols = table.get_columns();
        }
        else {
            for (auto& field : Dbms::executor.q.get_fields()) {
                for (auto& col : table.get_columns()) {
                    if (col.get_name() == field.value) {
                        result_cols.push_back(col);
                    }
                }
            }
        }
    }
    else {
        std::vector<size_t> matched_row_indices;
        auto& condition = conditions[0];
        Column condition_col = table.find_column_by_name(condition.get_field());
        if (condition_col.empty()) {
            return make_executor_error("at EXECUTION: column from WHERE clause does not exist");
        }

        const auto& all_rows = condition_col.get_rows();
        for (size_t i = 0; i < all_rows.size(); ++i) {
            if (predicate(get_operator(const_cast<Condition&>(condition)), all_rows[i], condition.get_operand2())) {
                matched_row_indices.push_back(i);
            }
        }

        auto requested_fields = Dbms::executor.q.get_fields();

        if (requested_fields.size() == 1 && requested_fields[0].value == "*") {
            requested_fields.clear();
            for (auto& col : table.get_columns()) {
                requested_fields.push_back({col.get_name(), col.get_type()});
            }
        }

        for (const auto& field : requested_fields) {
            Column original_col = table.find_column_by_name(field.value);
            if (original_col.empty()) {
                return make_executor_error("at EXECUTION: selected column '" + field.value + "' does not exist");
            }

            Column filtered_col(std::vector<Row>(), original_col.get_name(), original_col.get_type());
            for (size_t index : matched_row_indices) {
                if (index < original_col.get_rows().size()) {
                    filtered_col.add_row(original_col.get_rows()[index]);
                }
            }
            result_cols.push_back(filtered_col);
        }
    }


    auto group_by_cols_names = Dbms::executor.q.get_group_by_columns();

    if (!group_by_cols_names.empty()) {
        auto requested_fields = Dbms::executor.q.get_fields();
        auto grouped_cols = execute_group_by(result_cols, group_by_cols_names, requested_fields);
        ut_print(grouped_cols);
    }
    else {
        ut_print(result_cols);
    }

    return make_executor_error("");
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
        const std::string message = "at EXECUTION: unable to insert to nonexistent table";
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

std::vector<Column> Dbms::execute_group_by(std::vector<Column>& input_cols,
                                           const std::vector<std::string>& group_by_cols_names,
                                           const std::vector<Field>& requested_fields) {
    if (input_cols.empty() || group_by_cols_names.empty()) {
        return input_cols;
    }

    const std::string& group_by_col_name = group_by_cols_names[0];

    Column group_by_column;
    Column any_column_for_count_star;
    bool found = false;
    if (!input_cols.empty()) any_column_for_count_star = input_cols[0];

    for (auto& col : input_cols) {
        if (col.get_name() == group_by_col_name) {
            group_by_column = col;
            found = true;
        }
    }
    if (!found) return {};

    std::map<std::string, std::vector<size_t>> groups;
    for (size_t i = 0; i < group_by_column.get_rows().size(); ++i) {
        groups[group_by_column.get_rows()[i].get_data()].push_back(i);
    }

    std::vector<Column> final_result_cols;

    for (const auto& field : requested_fields) {
        if (field.agg_t == Aggregation_type::NONE) {
            if (field.value == group_by_col_name) {
                Column result_col({}, field.value, group_by_column.get_type());
                for (const auto& key : groups | std::views::keys) {
                    result_col.add_row(Row(key, return_data_type(key)));
                }
                final_result_cols.push_back(result_col);
            }
        }
        else if (field.agg_t == Aggregation_type::COUNT) {
            std::string new_col_name = "COUNT(" + field.value + ")";
            Column count_col({}, new_col_name, Data_type::INTEGER);
            for (const auto& value : groups | std::views::values) {
                size_t count = value.size();
                count_col.add_row(Row(std::to_string(count), Data_type::INTEGER));
            }
            final_result_cols.push_back(count_col);
        }
    }

    return final_result_cols;
}

std::pair<std::vector<Column>, Execution_error> Dbms::drop_table(std::string const& table_name) {
    if (!is_table_already_exist(table_name)) {
        return make_executor_error("at EXECUTION: unable to drop a nonexistent table");
    }
    auto it = std::find_if(Dbms::tables.begin(), Dbms::tables.end(),
                           [&table_name](Table& t) {
                               return t.get_table_name() == table_name;
                           });
    Dbms::tables.erase(it);
    fmt::println("dropped table: {}", table_name);
    return make_executor_error("");
}
