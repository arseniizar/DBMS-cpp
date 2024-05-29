#include "../Parser.hpp"

void Parser::step_delete_from_table() {
    auto is_table_name = Parser::peek_is_table_name("at DELETE: expected the name of the Table");
    if (!is_table_name) return;
    Parser::step = Step::where;
}