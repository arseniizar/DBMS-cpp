#include "../Parser.hpp"
#include "../../utils/ut_is_identifier/ut_is_identifier.hpp"

void Parser::step_foreign_key_as_type() {
    auto foreign_key = Parser::peek();
    if (foreign_key != "FOREIGN KEY") {
        Parser::step = Step::error;
        Parser::error_message = "expected a keyword FOREIGN KEY after the type of the Field";
        return;
    }
    Parser::q.append_field(Field(Parser::current_create_table_field_val,
                                 Parser::current_field_data_type,
                                 Key_attr(Key_type::FK)));
    Parser::pop();
    Parser::step = Step::references;
}

void Parser::step_references() {
    auto references = Parser::peek();
    if (references != "REFERENCES") {
        Parser::step = Step::error;
        Parser::error_message = "expected a keyword REFERENCES after the FOREIGN KEY keyword";
        return;
    }
    Parser::pop();
    Parser::step = Step::references_table;
}

void Parser::step_references_table() {
    auto is_table_name = peek_is_table_name("at REFERENCES: expected the name a the Table");
    if (!is_table_name) return;
    Parser::step = Step::references_table_opening_parens;
}

void Parser::step_references_table_opening_parens() {
    auto is_opening_parens = Parser::peek_is_opening_parens(
            "at REFERENCES: expected an opening parens after the Table name");
    if (!is_opening_parens) return;
    Parser::step = Step::references_field;
}

void Parser::step_references_field() {
    auto identifier = Parser::peek();
    if (!is_identifier(identifier)) {
        Parser::step = Step::error;
        Parser::error_message = "at REFERENCES: expected a proper name for the the Field";
        return;
    }
    // I will check this referencing fields in the Dbms before putting it into the Executor
    Parser::q.append_referenced_field_name(identifier);
    Parser::pop();
    Parser::step = Step::references_comma_or_closing_parens;
}

void Parser::step_references_comma_or_closing_parens() {
    auto comma_or_closing_parens = Parser::peek();
    if (comma_or_closing_parens != "," and comma_or_closing_parens != ")") {
        Parser::step = Step::error;
        Parser::error_message = "at REFERENCES: expected a comma or a closing parens";
    }
    auto current_field = Parser::q.get_current_field();
    current_field.k_a.add_ref(Ref(Parser::q.get_referenced_table_name(),
                                  Parser::q.get_referenced_fields_names().back()));
    Parser::q.set_current_field(current_field);
    if (comma_or_closing_parens == ",") {
        Parser::step = Step::references_field;
        return;
    }
    Parser::pop_flag = true;
    Parser::step = Step::create_table_comma_or_closing_parens;
    Parser::pop();
}