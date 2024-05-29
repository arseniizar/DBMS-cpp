//
// Created by Altezza on 29.04.2024.
//

// Source:
// https://marianogappa.github.io/software/2019/06/05/lets-build-a-sql-parser-in-go/

#include "Parser.hpp"
#include "fmt/core.h"

Parse_error Parser::get_error() {
    return error;
}

void Parser::clean() {
    Parser::sql = std::string();
    Parser::next_update_field = std::string();
    Parser::current_create_table_field_val = std::string();
    Parser::q = Query();
    Parser::index = std::size_t(0);
    Parser::step = Step::type;
}

void Parser::clean_error() {
    Parser::error = Parse_error();
}

void Parser::input(std::string const &s) {
    Parser::sql = s;
}

std::pair<struct Query, struct Parse_error> Parser::do_parse() {
    Parser::error_message = "";
    return Parser::parser_switch();
}
