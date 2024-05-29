#include "../Parser.hpp"

std::pair<Query, Parse_error> Parser::step_error() {
    return Parser::make_error_pair(Parser::error_message);
}