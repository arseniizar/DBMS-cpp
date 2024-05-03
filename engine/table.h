//
// Created by Altezza on 29.04.2024.
//

#ifndef DATABASEPROJECT_TABLE_H
#define DATABASEPROJECT_TABLE_H

// template structs for the tables, rows and columns
// I think I will use deque for that because it is more flexible...

#include <deque>
#include <string>

struct row {
private:
    std::string data;
public:
    auto get_data() {
        return row::data;
    }

    void insert_data(std::string const& ins_data) {
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

    void insert_row(row row, std::string const& data) {
        row.insert_data(data);
        column::rows.push_back(row);
    }

    void set_name(const std::string &name) {
        column::col_name = name;
    }

    const auto get_name() {
        return column::col_name;
    }
};

struct table {
private:
    std::string table_name;
    std::deque<column> columns;
public:
    auto get_columns() {
        return table::columns;
    }

    auto get_table_name() {
        return table::table_name;
    }

    void insert_column(column column, std::string const& col_name) {
        column.set_name(col_name);
        table::columns.push_back(column);
    }

    auto get_column_by_name(std::string const& col_name) {
        for(auto &column : columns)
            if(column.get_name() ==col_name) return column;
    }
};

#endif //DATABASEPROJECT_TABLE_H
