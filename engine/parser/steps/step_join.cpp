#include "../Parser.hpp"
#include "../../utils/ut_str_toupper/ut_str_toupper.hpp"
#include "../../utils/ut_is_identifier/ut_is_identifier.hpp"

void Parser::step_join() {
    auto join = Parser::peek();
    if (join != "JOIN") {
        Parser::step = Step::error;
        Parser::error_message = "expected a JOIN after the end of the previous statement";
        return;
    }
    str_toupper(join);
    auto join_type_str = return_join_type_str(join);
    join_type j_t = return_join_type(join_type_str);
    Parser::q.set_join_type(j_t);
    Parser::pop();
    Parser::step = Step::join_table;
}

void Parser::step_join_table() {
    auto is_table_name = Parser::peek_is_table_name("at JOIN: expected a Table name");
    if (!is_table_name) return;
    Parser::step = Step::join_on;
}

void Parser::step_join_on() {
    auto on = Parser::peek();
    if (on != "ON") {
        Parser::step = Step::error;
        Parser::error_message = "at JOIN: expected a keyword ON";
        return;
    }
    Parser::pop();
    Parser::step = Step::join_on_field1;
}

void Parser::step_join_on_field1() {
    auto identifier = Parser::peek();
    if (!is_identifier(identifier)) {
        Parser::step = Step::error;
        Parser::error_message = "at JOIN: expected a Field";
        return;
    }
    Parser::q.append_condition(Condition(identifier, true));
    Parser::pop();
    Parser::step = Step::join_on_equal;
}

void Parser::step_join_on_equal() {
    auto is_op = peek_is_operator("at WHERE: expected a WHERE operator");
    if (!is_op) return;
    Parser::step = Step::join_on_field2;
}

void Parser::step_join_on_field2() {
    auto identifier = Parser::peek();
    auto curr_cond = Parser::q.get_current_condition();
    if (!is_identifier(identifier)) {
        Parser::step = Step::error;
        Parser::error_message = "at JOIN: expected a Field";
        return;
    }
    curr_cond.operand2 = identifier;
    curr_cond.operand2_is_field = true;
    Parser::q.set_current_condition(curr_cond);
    Parser::pop();
    Parser::pop_flag = true;
}