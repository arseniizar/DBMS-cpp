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


struct row {
private:
    std::string data;
    data_type type;
public:
    auto get_data() {
        return row::data;
    }

    auto get_type() {
        return row::type;
    }

    void insert_data(std::string const &ins_data) {
        row::data = ins_data;
    }
};

struct column {
private:
    std::deque<row> rows;
    std::string col_name;
public:
    auto get_rows() {
        return column::rows;
    }

    void insert_row(row row, std::string const &data) {
        row.insert_data(data);
        column::rows.push_back(row);
    }

    void set_name(const std::string &name) {
        column::col_name = name;
    }

    auto get_name() {
        return column::col_name;
    }
};

// Here in the table struct I can implement the printing function of the table

struct table {
private:
    std::string table_name;
    std::vector<column> columns;

    static auto predicate(std::string const &op, row row, std::string const &field) {
        if (op == "eq") {
            return (row.get_data() == field) && (row.get_type()
                                                 == return_data_type(field));
        } else if (op == "ne") {
            return (row.get_data() != field) && (row.get_type()
                                                 == return_data_type(field));
        } else if (op == "gt") {
            return (row.get_data() > field) && (row.get_type()
                                                == return_data_type(field));
        } else if (op == "lt") {
            return (row.get_data() < field) && (row.get_type()
                                                == return_data_type(field));
        } else if (op == "gte") {
            return (row.get_data() >= field) && (row.get_type()
                                                 == return_data_type(field));
        } else if (op == "lte") {
            return (row.get_data() <= field) && (row.get_type()
                                                 == return_data_type(field));
        }
    }

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

    auto erase(const std::vector<condition> &conditions) {
        // I am using condition WHERE!
        auto erased_columns = std::vector<row>();
        for (auto const &condition: conditions) {
            auto col = table::get_column_by_name(condition.operand1);
            for (auto i = 0; i < col.get_rows().size(); i++) {
                auto op = get_operator(condition);
                // HOLY FUCKING HELL
                // assuming that operand1 - name, operand2 - int, etc.
                if (predicate(op, col.get_rows()[i], condition.operand2)) {
                    // make something for erasing some values inside the column
                }
            }
        }
        return erased_columns;
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
