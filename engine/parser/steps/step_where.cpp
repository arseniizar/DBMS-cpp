#include "../Parser.hpp"
#include "../../utils/ut_is_identifier/ut_is_identifier.hpp"
#include "../../utils/ut_find_in_vector/ut_find_in_vector.hpp"

void Parser::step_where() {
    auto where = Parser::peek();
    str_toupper(where);
    if (where != "WHERE") {
        Parser::step = Step::error;
        Parser::error_message = "expected a keyword WHERE after the end of the previous statement";
        return;
    }
    Parser::pop();
    Parser::step = Step::where_field;
}

void Parser::step_where_field() {
    auto identifier = Parser::peek();
    if (!is_identifier(identifier)) {
        Parser::step = Step::error;
        Parser::error_message = "at WHERE: expected a Field";
        return;
    }
    auto found_field = Parser::q.find_field_by_value(identifier);
    if (found_field.empty()) {
        Parser::step = Step::error;
        Parser::error_message = "at WHERE: expected an existing field";
        return;
    }
    Parser::q.append_condition(
            Condition(identifier, true, found_field.d_t));
    Parser::pop();
    Parser::step = Step::where_operator;
}

void Parser::step_where_operator() {
    auto is_op = peek_is_operator("at WHERE: expected a WHERE operator");
    if (!is_op) return;
    Parser::step = Step::where_value;
}

void Parser::step_where_value() {
    auto curr_cond = Parser::q.get_current_condition();
    auto identifier = Parser::peek();
    if (is_identifier(identifier)) {
        curr_cond.operand2 = identifier;
        curr_cond.operand2_is_field = true;
    } else {
        auto pair = Parser::peek_quoted_with_length();
        if (pair.second == 0) {
            Parser::step = Step::error;
            Parser::error_message = "at WHERE: expected a value";
            return;
        }
        curr_cond.operand2 = pair.first;
        curr_cond.operand2_is_field = false;
    }
    Parser::q.set_current_condition(curr_cond);
    Parser::pop();
    Parser::step = Step::where_and;
}

void Parser::step_where_and() {
    auto and_word = Parser::peek();
    str_toupper(and_word);
    if (and_word != "AND") {
        Parser::step = Step::error;
        Parser::error_message = "at WHERE: expected a keyword AND";
        return;
    }
    Parser::pop();
    Parser::step = Step::where_field;
}