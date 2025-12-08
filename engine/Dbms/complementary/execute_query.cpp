#include <ranges>

#include "../Dbms.hpp"
#include "../../utils/ut_contains_elems/ut_contains_elems.hpp"
#include "../../utils/ut_print/ut_print.hpp"
#include <set>
#include <map>

void Dbms::update_recent_change(Query& q) {
    const auto rec_c = recent_change(&q);
    Dbms::add_rec_change(rec_c);
    Dbms::queries.push_back(q);
    if (!Dbms::executor.get_error().message.empty())
        fmt::println("{}", Dbms::executor.get_error().message);
    else fmt::println("EXECUTED SUCCESSFULLY");
}


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
    case Q_action::UPDATE: {
        pair = Dbms::execute_update();
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
        if (!table_name.empty()) {
            Table table = Dbms::find_table_by_name(table_name);
            auto are_cols_consistent = ut_contains_elems(
                table.get_columns_names(),
                Table("func", pair.first).get_columns_names()
            );
            if (!are_cols_consistent
                and Dbms::executor.q.get_query_type() != Query_type::Select) {
                Dbms::executor.error = Execution_error("at EXECUTION: provided fields do not exist in the table");
            }
            q.set_p_table(&table);
        }
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

    std::vector<Column> where_filtered_cols;
    std::vector<Condition> where_conditions;
    std::vector<Condition> having_conditions;

    for (const auto& cond : conditions) {
        if (cond.c_type == Condition_type::WHERE) {
            where_conditions.push_back(cond);
        }
        else {
            having_conditions.push_back(cond);
        }
    }


    if (where_conditions.empty()) {
        where_filtered_cols = table.get_columns();
    }
    else {
        std::vector<size_t> matched_row_indices;
        auto& condition = where_conditions[0];
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

        for (auto& original_col : table.get_columns()) {
            Column filtered_col({}, original_col.get_name(), original_col.get_type());
            for (size_t index : matched_row_indices) {
                if (index < original_col.get_rows().size()) {
                    filtered_col.add_row(original_col.get_rows()[index]);
                }
            }
            where_filtered_cols.push_back(filtered_col);
        }
    }

    auto group_by_cols_names = Dbms::executor.q.get_group_by_columns();
    std::vector<Column> final_cols;

    if (!group_by_cols_names.empty()) {
        auto requested_fields = Dbms::executor.q.get_fields();
        final_cols = execute_group_by_with_having(where_filtered_cols, group_by_cols_names, requested_fields,
                                                  having_conditions);
    }
    else {
        if (!having_conditions.empty()) {
            return make_executor_error("at EXECUTION: HAVING clause requires a GROUP BY clause");
        }
        if (auto requested_fields = Dbms::executor.q.get_fields(); requested_fields.size() == 1 && requested_fields[0].
            value == "*") {
            final_cols = where_filtered_cols;
        }
        else {
            for (const auto& field : requested_fields) {
                bool found = false;
                for (auto& col : where_filtered_cols) {
                    if (field.value == col.get_name()) {
                        final_cols.push_back(col);
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    return make_executor_error("at EXECUTION: column '" + field.value + "' not found in table");
                }
            }
        }
    }

    ut_print(final_cols);
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

std::pair<std::vector<Column>, Execution_error> Dbms::execute_update() {
    const auto table_name = executor.q.get_table_name();
    if (!is_table_already_exist(table_name)) {
        return make_executor_error("at EXECUTION: table '" + table_name + "' does not exist");
    }

    auto& table_to_update = *std::ranges::find_if(tables,
                                                  [&](Table& t) { return t.get_table_name() == table_name; });

    auto updates = executor.q.get_updates();
    auto conditions = executor.q.get_conditions();

    auto& table_columns = table_to_update.get_columns_ref();

    for (const auto& u : updates) {
        bool column_exists = false;
        for (auto& c : table_columns) {
            if (c.get_name() == u.field_to_update.value) {
                column_exists = true;
                break;
            }
        }
        if (!column_exists) {
            return make_executor_error(
                "at EXECUTION: column '" + u.field_to_update.value + "' does not exist in table '" + table_name + "'");
        }
    }

    std::vector<size_t> row_indices_to_update;

    if (conditions.empty()) {
        if (!table_columns.empty()) {
            for (size_t i = 0; i < table_columns[0].get_rows().size(); ++i) {
                row_indices_to_update.push_back(i);
            }
        }
    }
    else {
        auto& condition = conditions[0];
        Column* condition_col = nullptr;
        for (auto& col : table_columns) {
            if (col.get_name() == condition.get_field()) {
                condition_col = &col;
                break;
            }
        }

        if (!condition_col) {
            return make_executor_error(
                "at EXECUTION: column '" + condition.get_field() + "' from WHERE clause does not exist");
        }

        const auto& rows = condition_col->get_rows();
        for (size_t i = 0; i < rows.size(); ++i) {
            if (predicate(get_operator(const_cast<Condition&>(condition)), rows[i], condition.get_operand2())) {
                row_indices_to_update.push_back(i);
            }
        }
    }

    for (size_t row_index : row_indices_to_update) {
        for (const auto& u : updates) {
            for (auto& table_col : table_columns) {
                if (table_col.get_name() == u.field_to_update.value) {
                    if (return_data_type(u.new_value) != table_col.get_type()) {
                        return make_executor_error(
                            "at EXECUTION: data type mismatch for column '" + table_col.get_name() + "'");
                    }
                    if (row_index < table_col.get_rows().size()) {
                        table_col.get_rows()[row_index] = Row(u.new_value, u.d_t);
                    }
                    break;
                }
            }
        }
    }

    return make_executor_error("");
}

std::vector<Column> Dbms::execute_group_by_with_having(std::vector<Column>& input_cols,
                                                       const std::vector<std::string>& group_by_cols_names,
                                                       const std::vector<Field>& requested_fields,
                                                       const std::vector<Condition>& having_conditions) {
    if (input_cols.empty() || group_by_cols_names.empty()) {
        return {};
    }

    const std::string& group_by_col_name = group_by_cols_names[0];
    Column* group_by_column = nullptr;
    for (auto& col : input_cols) {
        if (col.get_name() == group_by_col_name) {
            group_by_column = &col;
            break;
        }
    }
    if (!group_by_column) return {};

    std::map<std::string, std::vector<size_t>> groups;
    for (size_t i = 0; i < group_by_column->get_rows().size(); ++i) {
        groups[group_by_column->get_rows()[i].get_data()].push_back(i);
    }

    Table grouped_table;
    Column grouped_col({}, group_by_col_name, group_by_column->get_type());
    Column agg_col({}, "COUNT(*)", Data_type::INTEGER);

    for (auto const& [key, val] : groups) {
        grouped_col.add_row(Row(key, return_data_type(key)));
        agg_col.add_row(Row(std::to_string(val.size()), Data_type::INTEGER));
    }
    grouped_table.insert_column(grouped_col);
    grouped_table.insert_column(agg_col);

    if (!having_conditions.empty()) {
        const auto& condition = having_conditions[0];
        std::string operand1 = condition.operand1;
        std::transform(operand1.begin(), operand1.end(), operand1.begin(), ::toupper);

        Column* having_check_col = nullptr;
        if (operand1 == "COUNT(*)") {
            having_check_col = &agg_col;
        }
        else {
            having_check_col = &grouped_col;
        }

        std::vector<size_t> matched_indices;
        for (size_t i = 0; i < having_check_col->get_rows().size(); ++i) {
            if (predicate(get_operator(const_cast<Condition&>(condition)), having_check_col->get_rows()[i],
                          condition.operand2)) {
                matched_indices.push_back(i);
            }
        }

        Table final_table;
        for (auto& original_col : grouped_table.get_columns()) {
            Column filtered_col({}, original_col.get_name(), original_col.get_type());
            for (size_t index : matched_indices) {
                filtered_col.add_row(original_col.get_rows()[index]);
            }
            final_table.insert_column(filtered_col);
        }
        grouped_table = final_table;
    }

    std::vector<Column> final_result_cols;
    for (const auto& field : requested_fields) {
        std::string field_name = field.value;
        if (field.agg_t == Aggregation_type::COUNT) {
            field_name = "COUNT(" + field.value + ")";
            std::transform(field_name.begin(), field_name.end(), field_name.begin(), ::toupper);
        }

        for (auto& col : grouped_table.get_columns()) {
            std::string col_name_upper = col.get_name();
            std::transform(col_name_upper.begin(), col_name_upper.end(), col_name_upper.begin(), ::toupper);
            if (col.get_name() == field.value || col_name_upper == "COUNT(*)") {
                final_result_cols.push_back(col);
            }
        }
    }

    return final_result_cols;
}

std::pair<std::vector<Column>, Execution_error> Dbms::drop_table(std::string const& table_name) {
    if (!is_table_already_exist(table_name)) {
        return make_executor_error("at EXECUTION: unable to drop a nonexistent table");
    }
    const auto it = std::ranges::find_if(Dbms::tables,
                                         [&table_name](Table& t) {
                                             return t.get_table_name() == table_name;
                                         });
    Dbms::tables.erase(it);
    fmt::println("dropped table: {}", table_name);
    return make_executor_error("");
}
