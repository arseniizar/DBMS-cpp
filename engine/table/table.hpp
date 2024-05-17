//
// Created by Altezza on 29.04.2024.
//

#ifndef DATABASEPROJECT_TABLE_HPP
#define DATABASEPROJECT_TABLE_HPP

#include <deque>
#include <string>
#include <utility>
#include "../query/structs/condition.hpp"
#include "structs/data_type.hpp"
#include "../query/complementary/get_operator.hpp"
#include "column/column.hpp"
#include "row/row.hpp"

struct table {
private:
    std::string table_name;
    std::vector<column> columns;

public:
    table() = default;

    table(std::string table_name, const std::vector<column> &columns)
            : table_name(std::move(table_name)), columns(columns) {
        fmt::println("Hello Arina");
    }

    std::vector<column> get_columns();

    void set_table_name(std::string const &name);

    column get_column_by_name(std::string const &col_name);

    std::vector<column> erase(std::vector<condition> &conditions);

    std::vector<column> erase();

    std::string get_table_name();

    void insert_column(const column &column);
};

#endif //DATABASEPROJECT_TABLE_HPP
