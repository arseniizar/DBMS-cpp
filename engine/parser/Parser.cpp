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
    sql.clear();
    next_update_field.clear();
    current_create_table_field_val.clear();
    error_message.clear();
    q = Query();
    index = 0;
    step = Step::type;
    pop_flag = false;
    error = Parse_error();
}

void Parser::clean_error() {
    Parser::error = Parse_error();
    Parser::error_message = "";
}

void Parser::input(std::string const &s) {
    Parser::sql = s;
}

std::pair<struct Query, struct Parse_error> Parser::do_parse() {
    return Parser::parser_switch();
}
