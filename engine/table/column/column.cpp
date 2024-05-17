//
// Created by Altezza on 17.05.2024.
//
#include "column.hpp"

data_type column::get_type() {
    return column::type;
}

std::vector<row> column::get_rows() {
    return column::rows;
}

void column::remove_row(row &r) {
    for (auto iter = column::rows.begin(); iter != column::rows.end(); ++iter)
        if (r.get_data() == (*iter).get_data())
            column::rows.erase(iter);
}

void column::insert_row(row row, std::string const &data) {
    row.insert_data(data);
    column::rows.push_back(row);
}

void column::set_name(const std::string &name) {
    column::col_name = name;
}

std::string column::get_name() {
    return column::col_name;
}

// for the error cases, when the column is not found or something like that
bool column::empty() {
    return column::col_name.empty() && column::rows.empty();
}
