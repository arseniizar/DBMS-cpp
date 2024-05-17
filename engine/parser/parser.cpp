//
// Created by Altezza on 29.04.2024.
//

// Source:
// https://marianogappa.github.io/software/2019/06/05/lets-build-a-sql-parser-in-go/

#include "parser.hpp"
#include "fmt/core.h"

parse_error parser::get_error() {
    return error;
}

void parser::clean() {
    parser::sql = std::string();
    parser::next_update_field = std::string();
    parser::current_create_table_field = std::string();
    parser::q = query();
    parser::index = std::size_t(0);
    parser::step = step::type;
}

void parser::clean_error() {
    parser::error = parse_error();
}

void parser::input(std::string const &s) {
    parser::sql = s;
}

std::pair<struct query, struct parse_error> parser::do_parse() {
    return parser::parser_switch();
}
