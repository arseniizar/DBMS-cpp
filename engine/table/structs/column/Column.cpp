//
// Created by Altezza on 17.05.2024.
//
#include "Column.hpp"

Data_type Column::get_type() {
    return Column::type;
}

std::vector<Row> Column::get_rows() {
    return Column::rows;
}

void Column::remove_row(Row &r) {
    for (auto iter = Column::rows.begin(); iter != Column::rows.end(); ++iter)
        if (r.get_data() == (*iter).get_data())
            Column::rows.erase(iter);
}

void Column::add_row(Row row) {
    Column::rows.push_back(row);
}

void Column::set_name(const std::string &name) {
    Column::col_name = name;
}

std::string Column::get_name() {
    return Column::col_name;
}

// for the error cases, when the Column is not found or something like that
bool Column::empty() {
    return Column::col_name.empty() && Column::rows.empty();
}

void Column::set_rows(std::vector<Row> r_v) {
    Column::rows = std::move(r_v);
}

void Column::set_type(Data_type t) {
    Column::type = t;
}