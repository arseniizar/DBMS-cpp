#include "../Parser.hpp"

void Parser::step_drop_table() {
    auto is_table_name = peek_is_table_name("at DROP: expected a table name");
    if (!is_table_name) return;
    if(!Parser::peek().empty()) {
        Parser::error_message = "at DROP: improper continuation after name of the table";
        Parser::step = Step::error;
        return;
    }
    Parser::pop_flag = true;
}