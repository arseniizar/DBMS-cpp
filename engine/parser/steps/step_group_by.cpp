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
    auto identifier = Parser::peek();
    str_toupper(identifier);
    std::string current_select_field_val = Parser::q.get_current_select_field().value;
    if (!is_identifier(identifier) or identifier == current_select_field_val) {
        Parser::step = Step::error;
        Parser::error_message = "at GROUP BY: expected a Field to group by";
        return;
    }
    Parser::pop();
    if (is_peek_empty()) Parser::pop_flag = true;
    Parser::step = Step::having;
}