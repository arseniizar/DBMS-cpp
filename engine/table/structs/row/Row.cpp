//
// Created by Altezza on 17.05.2024.
//
#include "Row.hpp"

std::string Row::get_data() {
    return Row::data;
}

Data_type Row::get_type() {
    return Row::type;
}

void Row::insert_data(std::string const &ins_data) {
    Row::data = ins_data;
}

void Row::set_type(Data_type d_t) {
    Row::type = d_t;
}

bool Row::empty() {
    return Row::data.empty() and Row::type == Data_type::UNKNOWN;
}
