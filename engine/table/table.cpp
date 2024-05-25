//
// Created by Altezza on 17.05.2024.
//
#include "table.hpp"

std::vector<column> table::get_columns() {
    return table::columns;
}

void table::set_table_name(std::string const &name) {
    table::table_name = name;
}

column table::get_column_by_name(std::string const &col_name) {
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
        auto col = table::get_column_by_name(condition.get_operand1());
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
