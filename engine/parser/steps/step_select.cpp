#include "../Parser.hpp"
#include "../../utils/ut_is_identifier/ut_is_identifier.hpp"

void Parser::step_select_field() {
    const auto identifier = Parser::peek();
    if (!is_identifier_or_asterisk(identifier)) {
        Parser::step = Step::error;
        Parser::error_message = "at SELECT: expected a Field to select";
        return;
    }
    Parser::q.append_field(Field(identifier, Data_type::TABLE_SELECT));
    Parser::pop();
    if (const auto from = Parser::peek(); from == "FROM" or identifier == "*") {
        Parser::step = Step::select_from;
    }
    else {
        Parser::step = Step::select_comma;
    }
}

void Parser::step_select_comma() {
    auto isComma = Parser::peek_is_comma("at SELECT: expected a comma after the Field to select");
    if (!isComma) return;
    Parser::step = Step::select_field;
}

void Parser::step_select_from() {
    auto from = Parser::peek();
    str_toupper(from);
    if (from != "FROM") {
        Parser::step = Step::error;
        Parser::error_message = "at SELECT: expected a keyword FROM before the name of the Table";
        return;
    }
    Parser::pop();
    Parser::step = Step::select_from_table;
}

void Parser::step_select_from_table() {
    auto is_table_name = Parser::peek_is_table_name("at SELECT: expected a name of the Table");
    if (!is_table_name) return;

    auto next_token = peek();
    str_toupper(next_token);

    if (next_token.empty()) {
        pop_flag = true;
    }
    else if (next_token == "WHERE") {
        step = Step::where;
    }
    else if (next_token == "GROUP_BY") {
        step = Step::group_by;
    }
    else {
        error_message = "Unexpected token after table name in SELECT statement";
        step = Step::error;
    }
}
