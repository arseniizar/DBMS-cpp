#include "../Parser.hpp"
#include "../../utils/ut_is_identifier/ut_is_identifier.hpp"

void Parser::step_where() {
    auto where_token = pop();
    str_toupper(where_token);
    if (where_token != "WHERE") {
        Parser::step = Step::error;
        Parser::error_message = "Internal parser error: expected 'WHERE'";
        return;
    }
    Parser::step = Step::where_field;
}

void Parser::step_where_field() {
    auto identifier = Parser::peek();
    if (!is_identifier(identifier)) {
        Parser::step = Step::error;
        Parser::error_message = "at WHERE: expected a Field";
        return;
    }
    Parser::q.append_condition(Condition(identifier, true));
    Parser::pop();
    Parser::step = Step::where_operator;
}

void Parser::step_where_operator() {
    if (!peek_is_operator("at WHERE: expected a WHERE operator")) {
        return;
    }
    Parser::step = Step::where_value;
}

void Parser::step_where_value() {
    auto curr_cond = Parser::q.get_current_condition();

    if (const auto identifier = Parser::peek(); is_identifier(identifier)) {
        curr_cond.operand2 = identifier;
        curr_cond.operand2_is_field = true;
    }
    else {
        auto pair = Parser::peek_quoted_with_length();
        if (pair.second == 0) {
            Parser::step = Step::error;
            Parser::error_message = "at WHERE: expected a value (quoted) or another field";
            return;
        }
        curr_cond.operand2 = pair.first;
        curr_cond.operand2_is_field = false;
    }
    Parser::q.set_current_condition(curr_cond);
    Parser::pop();

    auto next_token = peek();
    str_toupper(next_token);

    if (next_token == "AND") {
        Parser::step = Step::where_and;
    }
    else if (next_token == "GROUP BY") {
        Parser::step = Step::group_by;
    }
    else if (next_token.empty()) {
        pop_flag = true;
    }
    else {
        pop_flag = true;
    }
}

void Parser::step_where_and() {
    Parser::pop();
    Parser::step = Step::where_field;
}
