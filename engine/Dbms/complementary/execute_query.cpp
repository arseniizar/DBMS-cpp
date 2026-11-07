#include <ranges>

#include "../Dbms.hpp"
#include "../../utils/ut_contains_elems/ut_contains_elems.hpp"
#include "../../utils/ut_print/ut_print.hpp"
#include <set>
#include <map>
#include <unordered_map>

void Dbms::update_recent_change(Query& q) {
    const auto rec_c = recent_change(&q);
    Dbms::add_rec_change(rec_c);
    Dbms::queries.push_back(q);
    if (!Dbms::executor.get_error().message.empty())
        fmt::println("{}", Dbms::executor.get_error().message);
    else fmt::println("EXECUTED SUCCESSFULLY");
}


std::pair<std::vector<Column>, Execution_error> Dbms::execute_query(Query& q) {
    executor.set_query(q);
    executor.execute();

    std::pair<std::vector<Column>, Execution_error> result_pair;

    switch (executor.action) {
        case Q_action::SELECT:
            result_pair = execute_select();
            break;
        case Q_action::INSERT:
            result_pair = execute_insert();
            break;
        case Q_action::DELETE:
            result_pair = execute_delete_from();
            break;
        case Q_action::UPDATE:
            result_pair = execute_update();
            break;
        case Q_action::CREATE:
            result_pair = execute_create_table();
            break;
        case Q_action::DROP:
            result_pair = drop_table(executor.q.get_table_name());
            break;
        default:
            result_pair = make_executor_error("Unknown query action");
            break;
    }

    if (!result_pair.second.message.empty()) {
        executor.set_error(result_pair.second);
        fmt::println("{}", result_pair.second.message);
    } else {
        fmt::println("EXECUTED SUCCESSFULLY");
    }

    queries.push_back(q);
    recent_changes.emplace_back(&queries.back());

    return result_pair;
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
        if (!are_relations_ok) {
            return make_executor_error("at EXECUTION: invalid foreign key reference");
        }
        Dbms::populate_keys();
    }
    Dbms::add_table(Dbms::executor.tmp_t);
    if (!Dbms::is_loading) ut_print(Dbms::executor.tmp_t);
    return std::make_pair(Dbms::executor.tmp_t.get_columns(), Execution_error());
}

std::pair<std::vector<Column>, Execution_error> Dbms::execute_select() {
    auto table_name = Dbms::executor.q.get_table_name();
    auto joined_table_name = Dbms::executor.q.get_joined_table_name();
    auto conditions = Dbms::executor.q.get_conditions();

    if (!is_table_already_exist(table_name)) {
        std::string message = "at EXECUTION: unable to select columns from nonexistent table";
        Dbms::executor.error = Execution_error(message);
        return make_executor_error(message);
    }
    if (!joined_table_name.empty() && !is_table_already_exist(joined_table_name)) {
        std::string message = "at EXECUTION: unable to join nonexistent table";
        Dbms::executor.error = Execution_error(message);
        return make_executor_error(message);
    }
    auto table = Dbms::find_table_by_name(table_name);
    auto are_consistent_error = Dbms::are_consistent().second;
    if (!are_consistent_error.message.empty()) {
        return make_executor_error(are_consistent_error.message);
    }

    std::vector<Condition> join_conditions;
    std::vector<Condition> where_conditions;
    std::vector<Condition> having_conditions;

    for (const auto& cond : conditions) {
        if (cond.c_type == Condition_type::JOIN_ON) {
            join_conditions.push_back(cond);
        } else if (cond.c_type == Condition_type::WHERE) {
            where_conditions.push_back(cond);
        } else {
            having_conditions.push_back(cond);
        }
    }

    std::vector<Column> source_cols;
    if (joined_table_name.empty()) {
        source_cols = table.get_columns();
    } else {
        auto right_table = Dbms::find_table_by_name(joined_table_name);
        if (join_conditions.empty()) {
            return make_executor_error("at EXECUTION: JOIN requires an ON condition");
        }
        const auto& join_cond = join_conditions[0];
        Column left_key_col = table.find_column_by_name(join_cond.operand1);
        Column right_key_col = right_table.find_column_by_name(join_cond.operand2);
        if (left_key_col.empty() || right_key_col.empty()) {
            left_key_col = table.find_column_by_name(join_cond.operand2);
            right_key_col = right_table.find_column_by_name(join_cond.operand1);
        }
        if (left_key_col.empty() || right_key_col.empty()) {
            return make_executor_error("at EXECUTION: invalid JOIN ON columns");
        }

        std::unordered_map<std::string, std::vector<size_t>> right_hash_index;
        const auto& right_key_rows = right_key_col.get_rows();
        right_hash_index.reserve(right_key_rows.size());
        for (size_t r_idx = 0; r_idx < right_key_rows.size(); ++r_idx) {
            right_hash_index[right_key_rows[r_idx].get_data()].push_back(r_idx);
        }

        auto left_cols = table.get_columns();
        auto right_cols = right_table.get_columns();
        for (auto& col : left_cols) {
            source_cols.emplace_back(std::vector<Row>{}, col.get_name(), col.get_type());
        }
        for (auto& col : right_cols) {
            source_cols.emplace_back(std::vector<Row>{}, col.get_name(), col.get_type());
        }

        const auto& left_key_rows = left_key_col.get_rows();
        const bool is_left_join = Dbms::executor.q.get_join_type() == join_type::LEFT_JOIN;

        for (size_t l_idx = 0; l_idx < left_key_rows.size(); ++l_idx) {
            auto it = right_hash_index.find(left_key_rows[l_idx].get_data());
            if (it != right_hash_index.end()) {
                for (size_t r_idx : it->second) {
                    for (size_t c = 0; c < left_cols.size(); ++c) {
                        source_cols[c].add_row(left_cols[c].get_rows()[l_idx]);
                    }
                    for (size_t c = 0; c < right_cols.size(); ++c) {
                        source_cols[left_cols.size() + c].add_row(right_cols[c].get_rows()[r_idx]);
                    }
                }
            } else if (is_left_join) {
                for (size_t c = 0; c < left_cols.size(); ++c) {
                    source_cols[c].add_row(left_cols[c].get_rows()[l_idx]);
                }
                for (size_t c = 0; c < right_cols.size(); ++c) {
                    source_cols[left_cols.size() + c].add_row(Row("", right_cols[c].get_type()));
                }
            }
        }
    }

    std::vector<Column> where_filtered_cols;
    if (where_conditions.empty()) {
        where_filtered_cols = source_cols;
    } else {
        size_t num_rows = source_cols.empty() ? 0 : source_cols[0].get_rows().size();
        std::vector<size_t> matched_row_indices;
        matched_row_indices.reserve(num_rows);

        std::vector<const Column*> cond_cols;
        cond_cols.reserve(where_conditions.size());
        for (const auto& cond : where_conditions) {
            const Column* found_col = nullptr;
            for (const auto& col : source_cols) {
                if (col.get_name() == cond.operand1) {
                    found_col = &col;
                    break;
                }
            }
            if (!found_col) {
                return make_executor_error("at EXECUTION: column from WHERE clause does not exist");
            }
            cond_cols.push_back(found_col);
        }

        for (size_t i = 0; i < num_rows; ++i) {
            bool all_match = true;
            for (size_t c_idx = 0; c_idx < where_conditions.size(); ++c_idx) {
                auto& cond = const_cast<Condition&>(where_conditions[c_idx]);
                if (!predicate(get_operator(cond), cond_cols[c_idx]->get_rows()[i], cond.get_operand2())) {
                    all_match = false;
                    break;
                }
            }
            if (all_match) {
                matched_row_indices.push_back(i);
            }
        }

        for (auto& original_col : source_cols) {
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

    // ut_print(final_cols);
     return std::make_pair(final_cols, Execution_error{});
}

std::pair<std::vector<Column>, Execution_error> Dbms::execute_delete_from() {
    auto table_name = Dbms::executor.q.get_table_name();
    auto conditions = Dbms::executor.q.get_conditions();
    if (!is_table_already_exist(table_name)) {
        std::string message = "at EXECUTION: unable to delete from nonexistent table";
        Dbms::executor.error = Execution_error(message);
        return make_executor_error(message);
    }
    auto table = Dbms::find_table_by_name(table_name);
    auto are_consistent_error = Dbms::are_consistent().second;
    if (!are_consistent_error.message.empty()) {
        return make_executor_error(are_consistent_error.message);
    }
    if (!conditions.empty()) {
        auto& table_to_modify = *std::ranges::find_if(tables,
            [&](Table& t) { return t.get_table_name() == table_name; });
        auto& table_columns = table_to_modify.get_columns_ref();
        size_t num_rows = table_columns.empty() ? 0 : table_columns[0].get_rows().size();

        std::vector<size_t> rows_to_keep;
        for (size_t i = 0; i < num_rows; ++i) {
            bool matches_all = true;
            for (auto& cond : conditions) {
                Column* cond_col = nullptr;
                for (auto& col : table_columns) {
                    if (col.get_name() == cond.get_field()) {
                        cond_col = &col;
                        break;
                    }
                }
                if (!cond_col || !predicate(get_operator(cond), cond_col->get_rows()[i], cond.get_operand2())) {
                    matches_all = false;
                    break;
                }
            }
            if (!matches_all) {
                rows_to_keep.push_back(i);
            }
        }

        for (auto& col : table_columns) {
            std::vector<Row> kept_rows;
            for (size_t idx : rows_to_keep) {
                if (idx < col.get_rows().size()) {
                    kept_rows.push_back(col.get_rows()[idx]);
                }
            }
            col.set_rows(kept_rows);
        }
        return std::make_pair(table_columns, Execution_error());
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

    std::unordered_map<std::string, std::vector<size_t>> groups;
    std::vector<std::string> group_keys;
    const auto& group_rows = group_by_column->get_rows();
    groups.reserve(group_rows.size());
    for (size_t i = 0; i < group_rows.size(); ++i) {
        const std::string& key = group_rows[i].get_data();
        auto [it, inserted] = groups.try_emplace(key);
        if (inserted) {
            group_keys.push_back(key);
        }
        it->second.push_back(i);
    }

    Table grouped_table;
    Column grouped_col({}, group_by_col_name, group_by_column->get_type());
    Column agg_col({}, "COUNT(*)", Data_type::INTEGER);

    for (const auto& key : group_keys) {
        const auto& val = groups[key];
        grouped_col.add_row(Row(key, return_data_type(key)));
        agg_col.add_row(Row(std::to_string(val.size()), Data_type::INTEGER));
    }
    grouped_table.insert_column(grouped_col);
    grouped_table.insert_column(agg_col);

    for (const auto& field : requested_fields) {
        if (field.agg_t != Aggregation_type::SUM &&
            field.agg_t != Aggregation_type::MIN &&
            field.agg_t != Aggregation_type::MAX) {
            continue;
        }
        Column* target_col = nullptr;
        for (auto& col : input_cols) {
            if (col.get_name() == field.value) {
                target_col = &col;
                break;
            }
        }
        if (!target_col) continue;

        std::string prefix = field.agg_t == Aggregation_type::SUM ? "SUM(" :
                             (field.agg_t == Aggregation_type::MIN ? "MIN(" : "MAX(");
        Column fn_col({}, prefix + field.value + ")", Data_type::INTEGER);

        for (const auto& key : group_keys) {
            const auto& row_indices = groups[key];
            long long acc = 0;
            bool first = true;
            for (size_t idx : row_indices) {
                long long v = std::stoll(target_col->get_rows()[idx].get_data());
                if (field.agg_t == Aggregation_type::SUM) {
                    acc += v;
                } else if (field.agg_t == Aggregation_type::MIN) {
                    if (first || v < acc) acc = v;
                } else if (field.agg_t == Aggregation_type::MAX) {
                    if (first || v > acc) acc = v;
                }
                first = false;
            }
            fn_col.add_row(Row(std::to_string(acc), Data_type::INTEGER));
        }
        grouped_table.insert_column(fn_col);
    }

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
        std::string expected_col_name = field.value;
        if (field.agg_t == Aggregation_type::SUM) {
            expected_col_name = "SUM(" + field.value + ")";
        } else if (field.agg_t == Aggregation_type::MIN) {
            expected_col_name = "MIN(" + field.value + ")";
        } else if (field.agg_t == Aggregation_type::MAX) {
            expected_col_name = "MAX(" + field.value + ")";
        }

        for (auto& col : grouped_table.get_columns()) {
            std::string col_name_upper = col.get_name();
            std::transform(col_name_upper.begin(), col_name_upper.end(), col_name_upper.begin(), ::toupper);
            if (field.agg_t == Aggregation_type::COUNT && col_name_upper == "COUNT(*)") {
                final_result_cols.push_back(col);
                break;
            }
            if (field.agg_t != Aggregation_type::COUNT && col.get_name() == expected_col_name) {
                final_result_cols.push_back(col);
                break;
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
