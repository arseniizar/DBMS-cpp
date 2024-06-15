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
    for (auto &col: Table::columns) {
        if (!col.get_rows().empty()) return false;
    }
    return true;
}

// cols -> col -> rows -> row
// every first row from every col
std::vector<std::vector<Row>> Table::get_rows_groups() {
    auto vec = std::vector<std::vector<Row>>();
    auto row_group = std::vector<Row>();
    size_t row_index = 0;
    while (true) {
        for (auto &col: Table::columns) {
            if (row_index < col.get_rows().size() and !col.get_rows()[row_index].empty())
                row_group.push_back(col.get_rows()[row_index]);
            else break;
        }
        if (row_group.empty()) break;
        vec.push_back(row_group);
        row_group.erase(row_group.begin(), row_group.end());
        row_index++;
    }
    return vec;
}

std::vector<Column> Table::find_cols_by_conditions(std::vector<Condition> conditions) {
    auto operand_fields = std::vector<std::string>();
    auto operand_values = std::vector<std::string>();
    auto operators = std::vector<Query_operator>();
    auto found_cols = std::vector<Column>();
    for (auto const &condition: conditions) {
        operators.push_back(condition._operator);
        if (condition.operand1_is_field) operand_fields.push_back(condition.operand1);
        else if (condition.operand2_is_field) operand_fields.push_back(condition.operand2);

    }
    for (auto &col: Table::get_columns()) {
        auto index_op = 0;
        auto index_val = 0;
        for (auto const &o_f: operand_fields) {
            std::vector<Row> found_rows;
            if (o_f == col.get_name()) {
                switch (operators.at(index_op)) {
                    case Query_operator::eq: {
                        for (auto &row: col.get_rows())
                            if (row.get_data() == operand_values.at(index_val)) found_rows.push_back(row);
                    }
                    case Query_operator::ne: {
                        for (auto &row: col.get_rows())
                            if (row.get_data() != operand_values.at(index_val)) found_rows.push_back(row);
                    }
                    case Query_operator::lt: {
                        for (auto &row: col.get_rows())
                            if (row.get_data() < operand_values.at(index_val)) found_rows.push_back(row);
                    }
                    case Query_operator::lte: {
                        for (auto &row: col.get_rows())
                            if (row.get_data() <= operand_values.at(index_val)) found_rows.push_back(row);
                    }
                    case Query_operator::gt: {
                        for (auto &row: col.get_rows())
                            if (row.get_data() > operand_values.at(index_val)) found_rows.push_back(row);
                    }
                    case Query_operator::gte: {
                        for (auto &row: col.get_rows())
                            if (row.get_data() >= operand_values.at(index_val)) found_rows.push_back(row);
                    }
                }
                index_op++;
                index_val++;
                Column new_col = Column(found_rows, col.get_name(), col.get_type());
                found_cols.push_back(new_col);
            }
        }
    }
    return found_cols;
}