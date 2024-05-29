#include "../Parser.hpp"
#include "../../utils/ut_is_identifier/ut_is_identifier.hpp"
#include "../../utils/ut_str_toupper/ut_str_toupper.hpp"

void Parser::step_having() {
    auto peeked = Parser::peek();
    str_toupper(peeked);
    if (peeked != "HAVING") {
        Parser::step = Step::error;
        Parser::error_message = "expected a keyword HAVING after GROUP BY";
    }
    Parser::pop();
    Parser::step = Step::having_field;
}

void Parser::step_having_field() {
    auto identifier = Parser::peek();
    if (!is_identifier(identifier)) {
        Parser::step = Step::error;
        Parser::error_message = "at HAVING: expected a Field to select";
        return;
    }
    Parser::pop();
    Parser::step = Step::having_operator;
}

void Parser::step_having_operator() {
    auto is_op = peek_is_operator("at HAVING: expected an operator");
    if (!is_op) return;
    Parser::step = Step::having_value;
}

void Parser::step_having_value() {
    auto identifier = Parser::peek();
    if (!is_identifier(identifier)) {
        Parser::step = Step::error;
        Parser::error_message = "at HAVING: expected a value";
        return;
    }
    Parser::pop();
    Parser::pop_flag = true;
}