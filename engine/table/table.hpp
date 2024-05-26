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
#include "structs/column/column.hpp"
#include "structs/row/row.hpp"
#include "structs/primary_key/primary_key.hpp"
#include "structs/foreign_key/foreign_key.hpp"

struct table {
private:
    std::string table_name;
    std::vector<column> columns;
    std::vector<primary_key> primary_keys;
    std::vector<foreign_key> foreign_keys;

public:
    table() = default;

    table(std::string table_name, const std::vector<column> &columns)
            : table_name(std::move(table_name)), columns(columns) {};

    std::vector<column> get_columns();

    void set_table_name(std::string const &name);

    column find_column_by_name(std::string const &col_name);

    std::vector<std::string> get_columns_names();

    std::vector<column> erase(std::vector<condition> &conditions);

    std::vector<column> erase();

    std::string get_table_name();

    void insert_column(const column &column);

    bool empty();

    void add_foreign_key(foreign_key const &fk);

    void add_primary_key(primary_key const &pk);

    bool contains_cols_names(const std::vector<std::string> &names);
};

#endif //DATABASEPROJECT_TABLE_HPP
