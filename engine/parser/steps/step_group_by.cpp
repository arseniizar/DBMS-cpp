#include "../Parser.hpp"
#include "../../utils/ut_str_toupper/ut_str_toupper.hpp"
#include "../../utils/ut_is_identifier/ut_is_identifier.hpp"

void Parser::step_group_by() {
    auto peeked = Parser::peek();
    str_toupper(peeked);
    if (peeked != "GROUP BY") {
        Parser::step = Step::error;
        Parser::error_message = "at GROUP BY: expected a GROUP BY";
        return;
    }
    Parser::pop();
    Parser::step = Step::group_by_field;
}

void Parser::step_group_by_field() {
    const auto identifier = pop();
    if (!is_identifier(identifier)) {
        step = Step::error;
        error_message = "at GROUP BY: expected a column name";
        return;
    }
    q.append_group_by_column(identifier);

    auto next_token = peek();
    str_toupper(next_token);
    if (next_token == "HAVING") {
        step = Step::having;
    }
    else if (next_token.empty()) {
        pop_flag = true;
    }
    else {
        step = Step::error;
        error_message = "Unexpected token after GROUP BY clause";
    }
}
