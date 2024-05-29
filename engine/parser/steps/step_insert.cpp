#include "../Parser.hpp"
#include "../../utils/ut_is_identifier/ut_is_identifier.hpp"
#include "../../utils/ut_str_toupper/ut_str_toupper.hpp"

void Parser::step_insert_table() {
    auto is_table_name = Parser::peek_is_table_name("at INSERT: expected a Table name");
    if (!is_table_name) return;
    Parser::step = Step::insert_fields_opening_parens;
}

void Parser::step_insert_fields_opening_parens() {
    auto opening_parens = Parser::peek();
    if (opening_parens == "VALUES") {
        Parser::step = Step::insert_values_rword;
        return;
    }
    if (opening_parens != "(" || opening_parens.size() != 1) {
        Parser::step = Step::error;
        Parser::error_message = "at INSERT: expected an opening parens";
        return;
    }
    Parser::pop();
    Parser::step = Step::insert_fields;
}

void Parser::step_insert_fields() {
    auto identifier = Parser::peek();
    if (!is_identifier(identifier)) {
        Parser::step = Step::error;
        Parser::error_message = "at INSERT: expected a Field to insert";
        return;
    }
    Parser::q.append_field(Field(identifier, Data_type::INSERT_COL));
    Parser::pop();
    Parser::step = Step::insert_fields_comma_or_closing_parens;
}

void Parser::step_insert_fields_comma_or_closing_parens() {
    auto comma_or_closing_paren = Parser::peek();
    if (comma_or_closing_paren != "," and comma_or_closing_paren != ")") {
        Parser::step = Step::error;
        Parser::error_message = "at INSERT: expected comma or closing parens";
        return;
    }
    Parser::pop();
    if (comma_or_closing_paren == ",") {
        Parser::step = Step::insert_fields;
        return;
    }
    Parser::step = Step::insert_values_rword;
}

void Parser::step_insert_values_opening_parens() {
    auto opening_parens = Parser::peek();
    if (opening_parens != "(") {
        Parser::step = Step::error;
        Parser::error_message = "at INSERT: expected an opening parens";
        return;
    }
    Parser::q.append_inserts_vec(std::vector<std::string>());
    Parser::pop();
    Parser::step = Step::insert_values;
}

void Parser::step_insert_values_rword() {
    auto values = Parser::peek();
    str_toupper(values);
    if (values != "VALUES") {
        Parser::step = Step::error;
        Parser::error_message = "at INSERT: expected keyword VALUES";
        return;
    }
    Parser::pop();
    Parser::step = Step::insert_values_opening_parens;
}

void Parser::step_insert_values() {
    auto pair = Parser::peek_quoted_with_length();
    if (pair.second == 0) {
        Parser::step = Step::error;
        Parser::error_message = "at INSERT: expected a value to insert";
        return;
    }
    Parser::q.append_insert(pair.first);
    Parser::pop();
    Parser::step = Step::insert_values_comma_or_closing_parens;
}

void Parser::step_insert_values_comma_or_closing_parens() {
    auto comma_or_closing_parens = Parser::peek();
    if (comma_or_closing_parens != "," and comma_or_closing_parens != ")") {
        Parser::step = Step::error;
        Parser::error_message = "at INSERT: expected a comma or a closing parens";
        return;
    }
    Parser::pop();
    if (comma_or_closing_parens == ",") {
        Parser::step = Step::insert_values;
        return;
    }
    auto curr_insert_row = Parser::q.get_current_inserts();
    if (curr_insert_row.size() < Parser::q.get_inserts_size()) {
        Parser::step = Step::error;
        Parser::error_message = "at INSERT: expected a group of inserts";
        return;
    }
    Parser::pop();
    if (is_peek_empty()) Parser::pop_flag = true;
    Parser::step = Step::insert_values_comma_before_opening_parens;
}

void Parser::step_insert_values_comma_before_opening_parens() {
    auto isComma = Parser::peek_is_comma("at INSERT: expected a comma before opening parens");
    if (!isComma) return;
    Parser::step = Step::insert_values_opening_parens;
}