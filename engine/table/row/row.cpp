//
// Created by Altezza on 17.05.2024.
//
#include "row.hpp"

std::string row::get_data() {
    return row::data;
}

data_type row::get_type() {
    return row::type;
}

void row::insert_data(std::string const &ins_data) {
    row::data = ins_data;
}