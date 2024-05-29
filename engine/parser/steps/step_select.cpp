#include "../Parser.hpp"
#include "../../utils/ut_is_identifier/ut_is_identifier.hpp"
#include "../../utils/ut_str_toupper/ut_str_toupper.hpp"

void Parser::step_select_field() {
    auto identifier = Parser::peek();
    if (!is_identifier_or_asterisk(identifier)) {
        Parser::step = Step::error;
        Parser::error_message = "at SELECT: expected a Field to select";
        return;
    }
    Parser::q.append_field(Field(identifier, Data_type::TABLE_SELECT));
    Parser::pop();
    auto from = Parser::peek();
    str_toupper(from);
    if (from == "AS") {
        Parser::pop();
        auto alias = Parser::peek();
        if (!is_identifier(alias)) {
            Parser::step = Step::error;
            Parser::error_message = "at SELECT: expected an alias after AS";
            return;
        }
        Parser::q.append_alias(identifier, alias);
        Parser::pop();
        from = Parser::peek();
    }
    str_toupper(from);
    if (from == "FROM") {
        Parser::step = Step::select_from;
    } else {
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
    Parser::set_additional_step();
}