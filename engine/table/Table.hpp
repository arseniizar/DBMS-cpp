//
// Created by Altezza on 29.04.2024.
//

#ifndef DATABASEPROJECT_TABLE_HPP
#define DATABASEPROJECT_TABLE_HPP

#include <deque>
#include <string>
#include <utility>
#include "../query/structs/Condition.hpp"
#include "structs/Data_type.hpp"
#include "../query/complementary/get_operator.hpp"
#include "structs/column/Column.hpp"
#include "structs/row/Row.hpp"
#include "structs/primary_key/Primary_key.hpp"
#include "structs/foreign_key/Foreign_key.hpp"

struct Table {
private:
    std::string table_name;
    std::vector<Column> columns;
    Primary_key primary_key;
    std::vector<Foreign_key> foreign_keys;

public:
    Table() = default;

    Table(std::string table_name, const std::vector<Column> &columns)
            : table_name(std::move(table_name)), columns(columns) {};

    std::vector<Column> get_columns();

    std::vector<Column>& get_columns_ref();

    void set_table_name(std::string const &name);

    Column find_column_by_name(std::string const &col_name);

    std::vector<std::string> get_columns_names();

    std::vector<Column> erase(std::vector<Condition> &conditions);

    std::vector<Column> erase();

    std::string get_table_name();

    void insert_column(const Column &column);

    bool empty();

    void add_foreign_key(Foreign_key const &fk);

    void set_primary_key(Primary_key const &pk);

    bool contains_cols_names(const std::vector<std::string> &names);

    Primary_key get_primary_key();

    std::vector<Foreign_key> get_foreign_keys();

    void set_columns(std::vector<Column> const& cols);

    void set_foreign_keys(std::vector<Foreign_key> const& fks);

    bool are_table_rows_empty();

    std::vector<std::vector<Row>> get_rows_groups();

    std::vector<Column> find_cols_by_conditions(std::vector<Condition> conditions);
};

#endif //DATABASEPROJECT_TABLE_HPP
