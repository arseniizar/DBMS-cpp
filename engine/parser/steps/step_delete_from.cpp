#include "../Parser.hpp"

void Parser::step_delete_from_table() {
    if (!peek_is_table_name("at DELETE: expected the name of the Table")) {
        return;
    }

    auto next_token = peek();
    str_toupper(next_token);

    if (next_token == "WHERE") {
        Parser::step = Step::where;
    } else if (next_token.empty()) {
        pop_flag = true;
    } else {
        Parser::step = Step::error;
        Parser::error_message = "at DELETE: unexpected token after table name";
    }
}
