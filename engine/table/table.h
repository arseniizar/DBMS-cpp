//
// Created by Altezza on 29.04.2024.
//

#ifndef DATABASEPROJECT_TABLE_H
#define DATABASEPROJECT_TABLE_H

// template structs for the tables, rows and columns
// I think I will use deque for that because it is more flexible...

#include <deque>
#include <string>
#include <utility>
#include "../query/condition.h"
#include "data_type.h"
#include "../query/get_operator.h"
#include "column.h"
#include "row.h"

// Here in the table struct I can implement the printing function of the table

struct table {
private:
    std::string table_name;
    std::vector<column> columns;

public:
    table(std::string table_name, const std::vector<column> &columns)
            : table_name(std::move(table_name)), columns(columns) {}

    auto get_columns() {
        return table::columns;
    }

    auto get_column_by_name(std::string const &col_name) {
        for (auto &column: columns)
            if (column.get_name() == col_name) return column;
    }

    auto erase(std::vector<condition> &conditions) {
        // I am using condition WHERE!
        auto cols_of_erased_rows = std::vector<column>();
        auto erased_rows = std::vector<row>();
        for (auto &condition: conditions) {
            auto col = table::get_column_by_name(condition.get_operand1());
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
            auto temp_col = column(erased_rows, col.get_name() + ".rec");
            cols_of_erased_rows.push_back(temp_col);
        }
        return cols_of_erased_rows;
    }

    auto erase() {
        auto erased_columns = std::vector<column>();
        for (auto &column: table::columns) erased_columns.push_back(column);
        table::columns.erase(table::columns.begin(), table::columns.end());
        return erased_columns;
    }

    auto get_table_name() {
        return table::table_name;
    }

    void insert_column(column column, std::string const &col_name) {
        column.set_name(col_name);
        table::columns.push_back(column);
    }
};

#endif //DATABASEPROJECT_TABLE_H
