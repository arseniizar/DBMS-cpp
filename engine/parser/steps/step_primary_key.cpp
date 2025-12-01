#include "../Parser.hpp"

void Parser::step_primary_key_as_type() {
    auto primary_key = Parser::peek();
    if (primary_key != "PRIMARY KEY") {
        Parser::step = Step::error;
        Parser::error_message = "expected a keyword PRIMARY KEY after the type of the Field";
        return;
    }
    // auto potential_pk = Parser::q.get_primary_key();
    // if(!potential_pk.empty()) {
    //     Parser::step = Step::error;
    //     Parser::error_message = "a table must have only one PRIMARY KEY";
    //     return;
    // }
    Parser::q.append_field(Field(Parser::current_create_table_field_val,
                                 Parser::current_field_data_type,
                                 Key_attr(Key_type::PK)));
    Parser::pop();
    Parser::step = Step::create_table_comma_or_closing_parens;
}