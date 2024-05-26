//
// Created by Altezza on 17.05.2024.
//
#include <algorithm>
#include "table.hpp"

std::vector<column> table::get_columns() {
    return table::columns;
}

void table::set_table_name(std::string const &name) {
    table::table_name = name;
}

column table::find_column_by_name(std::string const &col_name) {
    for (auto &column: columns)
        if (column.get_name() == col_name) return column;
    return {};
}

std::vector<column> table::erase(std::vector<condition> &conditions) {
    // I am using condition WHERE!
    auto cols_of_erased_rows = std::vector<column>();
    auto erased_rows = std::vector<row>();
    auto type = data_type::UNKNOWN;
    for (auto &condition: conditions) {
        auto col = table::find_column_by_name(condition.get_operand1());
        type = col.get_type();
        for (auto i = 0; i < col.get_rows().size(); i++) {
            auto op = get_operator(condition);
            auto row = col.get_rows()[i];
            // HOLY FUCKING HELL
            // assuming that operand1 - name, operand2 - int, etc.
            if (predicate(op, row, condition.get_operand2())) {
                // make something for erasing some values inside the column
                erased_rows.push_back(row);
                col.remove_row(row);
            }
        }
        auto temp_col = column(erased_rows, col.get_name() + ".rec", type);
        cols_of_erased_rows.push_back(temp_col);
    }
    return cols_of_erased_rows;
}

std::vector<column> table::erase() {
    auto erased_columns = std::vector<column>();
    for (auto &column: table::columns) erased_columns.push_back(column);
    table::columns.erase(table::columns.begin(), table::columns.end());
    return erased_columns;
}

std::string table::get_table_name() {
    return table::table_name;
}

void table::insert_column(const column &column) {
    table::columns.push_back(column);
}

bool table::empty() {
    return table::table_name.empty() && columns.empty();
}

std::vector<std::string> table::get_columns_names() {
    auto vec = std::vector<std::string>();
    std::for_each(table::columns.begin(), table::columns.end(), [&vec](column &col){
        vec.push_back(col.get_name());
    });
    return vec;
}

void table::add_foreign_key(foreign_key const &fk) {
    table::foreign_keys.push_back(fk);
}

void table::add_primary_key(primary_key const &pk) {
    table::primary_keys.push_back(pk);
}

bool table::contains_cols_names(const std::vector<std::string> &names) {
    return std::ranges::is_permutation(names, table::get_columns_names());
}
