#include "../Parser.hpp"
#include "../../utils/ut_is_identifier/ut_is_identifier.hpp"

void Parser::step_update_table() {
    auto is_table_name = Parser::peek_is_table_name("at UPDATE: expected a Table name");
    if (!is_table_name) return;
    Parser::step = Step::update_set;
}

void Parser::step_update_set() {
    auto set = Parser::peek();
    if (set != "SET") {
        Parser::step = Step::error;
        Parser::error_message = "at UPDATE: expected keyword SET";
        return;
    }
    Parser::pop();
    Parser::step = Step::update_field;
}

void Parser::step_update_field() {
    auto identifier = Parser::peek();
    if (!is_identifier(identifier)) {
        Parser::step = Step::error;
        Parser::error_message = "at UPDATE: expected a Field to update";
        return;
    }
    Parser::next_update_field = Parser::peek();
    Parser::pop();
    Parser::step = Step::update_equals;
}

void Parser::step_update_equals() {
    auto equals = Parser::peek();
    if (equals != "=") {
        Parser::step = Step::error;
        Parser::error_message = "at UPDATE: expected an '='";
        return;
    }
    Parser::pop();
    Parser::step = Step::update_value;
}

void Parser::step_update_value() {
    auto pair = Parser::peek_quoted_with_length();
    if (pair.second == 0) {
        Parser::step = Step::error;
        Parser::error_message = "at UPDATE: expected a value to update with";
        return;
    }
    Parser::q.append_update(Parser::next_update_field, pair.first);
    Parser::next_update_field.erase();
    Parser::pop();
    if (is_peek_empty()) Parser::pop_flag = true;
    else Parser::set_additional_step();
}

void Parser::step_update_comma() {
    auto isComma = Parser::peek_is_comma("at UPDATE: expected a comma");
    if (!isComma) return;
    Parser::step = Step::update_field;
}