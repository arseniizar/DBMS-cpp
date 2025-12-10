#include "../Parser.hpp"
#include "../../utils/ut_is_identifier/ut_is_identifier.hpp"

void Parser::step_select_field() {
    std::string identifier_or_func = peek();
    str_toupper(identifier_or_func);

    if (identifier_or_func == "COUNT") {
        pop();
        if (!peek_is_opening_parens("at SELECT: expected '(' after COUNT")) return;

        std::string col_name = pop();
        if (!is_identifier_or_asterisk(col_name)) {
            step = Step::error;
            error_message = "at SELECT: expected column name or '*' inside COUNT()";
            return;
        }

        if (auto closing_paren = pop(); closing_paren != ")") {
            step = Step::error;
            error_message = "at SELECT: expected ')' after column name in COUNT()";
            return;
        }

        Field agg_field;
        agg_field.value = col_name;
        agg_field.agg_t = Aggregation_type::COUNT;
        agg_field.d_t = Data_type::TABLE_SELECT;
        q.append_field(agg_field);
    }
    else {
        identifier_or_func = pop();
        if (!is_identifier_or_asterisk(identifier_or_func)) {
            step = Step::error;
            error_message = "at SELECT: expected a Field to select or aggregate function";
            return;
        }
        q.append_field({identifier_or_func, Data_type::TABLE_SELECT});
    }

    auto next_token = peek();
    str_toupper(next_token);

    if (next_token == "FROM") {
        step = Step::select_from;
    }
    else if (next_token == ",") {
        step = Step::select_comma;
    }
    else {
        step = Step::error;
        error_message = "at SELECT: expected ',' or 'FROM' after field";
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
    if (!peek_is_table_name("at SELECT: expected a name of the Table")) {
        return;
    }

    std::string next_token = peek();

    if (next_token == ";") {
        pop();
        next_token = peek();
    }

    if (next_token.empty()) {
        return;
    }

    str_toupper(next_token);

    if (next_token == "WHERE") {
        step = Step::where;
    }
    else if (next_token == "GROUP BY") {
        step = Step::group_by;
    }
    else {
        step = Step::error;
        error_message = "Unexpected token '" + peek() + "' after table name";
    }
}
