#include "../Parser.hpp"
#include "../../utils/ut_is_identifier/ut_is_identifier.hpp"

void Parser::step_primary_key_at_end() {
    auto primary_key = Parser::peek();
    if (primary_key != "PRIMARY KEY") {
        Parser::step = Step::error;
        Parser::error_message = "expected a keyword PRIMARY KEY after the type of the Field";
        return;
    }
    Parser::pop();
    Parser::step = Step::primary_key_opening_parens;
}

void Parser::step_primary_key_opening_parens() {
    auto is_opening_parens =
            peek_is_opening_parens("expected an opening parens after the PRIMARY KEY keyword");
    if (!is_opening_parens) return;
    Parser::step = Step::primary_key_field;
}

void Parser::step_primary_key_field() {
    auto identifier = Parser::peek();
    if (!is_identifier(identifier)) {
        Parser::step = Step::error;
        Parser::error_message = "expected a proper name of one of the fields from the Table";
        return;
    }
    bool is_field_present = std::any_of(Parser::q.get_fields().begin(),
                                        Parser::q.get_fields().end(),
                                        [&identifier](Field const &field) {
                                            return field.value == identifier;
                                        });
    if (!is_field_present) {
        Parser::step = Step::error;
        Parser::error_message = "Field with the name " + identifier + " doesn't exist in this Table";
        return;
    }
    auto field_iter = get_field_by_name(identifier);
    field_iter->k_a = Key_attr(Key_type::PK);
    Parser::pop();
    Parser::step = Step::primary_key_comma_or_closing_parens;
}

void Parser::step_primary_key_comma_or_closing_parens() {
    auto comma_or_closing_parens = Parser::peek();
    if (comma_or_closing_parens != "," and comma_or_closing_parens != ")") {
        Parser::step = Step::error;
        Parser::error_message = "expected a closing parens or comma after the Field inside PRIMARY KEY()";
        return;
    }
    if (comma_or_closing_parens == ",") {
        Parser::step = Step::primary_key_field;
        return;
    }
    Parser::pop_flag = true;
    Parser::step = Step::create_table_comma_or_closing_parens;
}

void Parser::step_foreign_key() {
    auto foreign_key = Parser::peek();
    if (foreign_key != "FOREIGN KEY") {
        Parser::step = Step::error;
        Parser::error_message = "expected a FOREIGN KEY keyword!";
        return;
    }
    Parser::pop();
    Parser::step = Step::foreign_key_opening_parens;
}

void Parser::step_foreign_key_opening_parens() {
    auto is_opening_parens = Parser::peek_is_opening_parens(
            "at FOREIGN KEY: expected an opening parens");
    if (!is_opening_parens) return;
    Parser::step = Step::foreign_key_field;
}

void Parser::step_foreign_key_field() {
    auto identifier = Parser::peek();
    if (!is_identifier(identifier)) {
        Parser::step = Step::error;
        Parser::error_message = "at FOREIGN KEY: expected a Field that will be a foreign key";
        return;
    }
    auto f = std::find_if(
            Parser::q.get_fields().begin(),
            Parser::q.get_fields().end(),
            [&identifier](Field const &f) {
                return f.value == identifier;
            }
    );
    f->k_a = Key_attr(Key_type::FK, Ref(current_references_table_name,
                                        f->value));
    Parser::pop();
    Parser::step = Step::foreign_key_comma_or_closing_parens;
}

void Parser::step_foreign_key_comma_or_closing_parens() {
    auto comma_or_closing_parens = Parser::peek();
    if (comma_or_closing_parens != "," and comma_or_closing_parens != ")") {
        Parser::step = Step::error;
        Parser::error_message = "at FOREIGN KEY: expected a comma or a closing parens";
        return;
    }
    if (comma_or_closing_parens == ",") {
        Parser::step = Step::foreign_key_field;
        return;
    }
    Parser::pop();
    Parser::step = Step::references;
}
