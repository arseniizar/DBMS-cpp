//
// Created by Altezza on 29.04.2024.
//

#ifndef DATABASEPROJECT_TABLE_H
#define DATABASEPROJECT_TABLE_H

// template structs for the tables, rows and columns
// I think I will use deque for that because it is more flexible...

#include <deque>
#include <string>

template<typename T>
struct t_row {
private:
    T data;
public:
    auto get_data() {
        return t_row::data;
    }

    void insert_data(T ins_data) {
        t_row::data = ins_data;
    }
};

template<typename T>
struct t_column {
private:
    std::deque<t_row<T>> rows;
    std::string col_name;
public:
    auto get_rows() {
        return t_column::rows;
    }

    void insert_row(t_row<T> row, T data) {
        row.insert_data(data);
        t_column::rows.push_back(row);
    }

    void set_name(const std::string &name) {
        t_column::col_name = name;
    }
};

template<typename T>
struct t_table {
private:
    std::deque<t_column<T>> columns;
public:
    auto get_columns() {
        return t_table::columns;
    }

    void insert_column(t_column<T> column, std::string col_name) {
        column.set_name(col_name);
        t_table::columns.push_back(column);
    }
};

#endif //DATABASEPROJECT_TABLE_H
