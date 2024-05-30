//
// Created by Altezza on 17.05.2024.
//
#include <algorithm>
#include "Table.hpp"

std::vector<Column> Table::get_columns() {
    return Table::columns;
}

void Table::set_table_name(std::string const &name) {
    Table::table_name = name;
}

Column Table::find_column_by_name(std::string const &col_name) {
    for (auto &column: columns)
        if (column.get_name() == col_name) return column;
    return {};
}

std::vector<Column> Table::erase(std::vector<Condition> &conditions) {
    // I am using Condition WHERE!
    auto cols_of_erased_rows = std::vector<Column>();
    auto erased_rows = std::vector<Row>();
    auto type = Data_type::UNKNOWN;
    for (auto &condition: conditions) {
        auto col = Table::find_column_by_name(condition.get_operand1());
        type = col.get_type();
        for (auto i = 0; i < col.get_rows().size(); i++) {
            auto op = get_operator(condition);
            auto row = col.get_rows()[i];
            // HOLY FUCKING HELL
            // assuming that operand1 - name, operand2 - int, etc.
            if (predicate(op, row, condition.get_operand2())) {
                // make something for erasing some values inside the Column
                erased_rows.push_back(row);
                col.remove_row(row);
            }
        }
        auto temp_col = Column(erased_rows, col.get_name() + ".rec", type);
        cols_of_erased_rows.push_back(temp_col);
    }
    return cols_of_erased_rows;
}

std::vector<Column> Table::erase() {
    auto erased_columns = std::vector<Column>();
    for (auto &column: Table::columns) erased_columns.push_back(column);
    Table::columns.erase(Table::columns.begin(), Table::columns.end());
    return erased_columns;
}

std::string Table::get_table_name() {
    return Table::table_name;
}

void Table::insert_column(const Column &column) {
    Table::columns.push_back(column);
}

bool Table::empty() {
    return Table::table_name.empty() && columns.empty();
}

std::vector<std::string> Table::get_columns_names() {
    auto vec = std::vector<std::string>();
    std::for_each(Table::columns.begin(), Table::columns.end(), [&vec](Column &col) {
        vec.push_back(col.get_name());
    });
    return vec;
}

void Table::add_foreign_key(Foreign_key const &fk) {
    Table::foreign_keys.push_back(fk);
}

void Table::set_primary_key(Primary_key const &pk) {
    Table::primary_key = pk;
}

bool Table::contains_cols_names(const std::vector<std::string> &names) {
    return std::ranges::is_permutation(names, Table::get_columns_names());
}

Primary_key Table::get_primary_key() {
    return Table::primary_key;
}

std::vector<Foreign_key> Table::get_foreign_keys() {
    return Table::foreign_keys;
}

void Table::set_columns(const std::vector<Column> &cols) {
    Table::columns = cols;
}

void Table::set_foreign_keys(const std::vector<Foreign_key> &fks) {
    Table::foreign_keys = fks;
}

bool Table::are_table_rows_empty() {
    return std::all_of(Table::columns.begin(), Table::columns.end(),
                       [](Column &col) {
                           return std::all_of(col.get_rows().begin(), col.get_rows().end(),
                                              [](Row &row) {
                                                  return row.empty();
                                              });
                       });
}

std::vector<std::vector<Row>> Table::get_rows_groups() {
    auto vec = std::vector<std::vector<Row>>();
    auto row_group = std::vector<Row>();
    size_t row_index = 0;
    for (auto i = 0; i < Table::columns.size(); ++i) {
        for (auto &col: Table::columns) {
            row_group.push_back(col.get_rows()[row_index]);
        }
        vec.push_back(row_group);
        row_group.erase(row_group.begin(), row_group.end());
        row_index++;
    }
    return vec;
}

// cols -> col -> rows -> row
// every first row from every col