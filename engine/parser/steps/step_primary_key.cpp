#include "../Parser.hpp"

void Parser::step_primary_key_as_type() {
    auto primary = Parser::peek();
    str_toupper(primary);
    if (primary != "PRIMARY") {
        Parser::step = Step::error;
        Parser::error_message = "expected a keyword PRIMARY after the type of the Field";
        return;
    }
    Parser::pop();

    auto key = Parser::peek();
    str_toupper(key);
    if (key != "KEY") {
        Parser::step = Step::error;
        Parser::error_message = "expected a keyword KEY after PRIMARY";
        return;
    }
    Parser::pop();

    auto fields = Parser::q.get_fields();

    const bool pk_exists = std::ranges::any_of(fields, [](const Field& field) {
       return field.k_a.k_t == Key_type::PK;
    });

    if (pk_exists) {
        Parser::step = Step::error;
        Parser::error_message = "a table can have only one PRIMARY KEY";
        return;
    }

    Parser::q.append_field(Field(Parser::current_create_table_field_val,
                                 Parser::current_field_data_type,
                                 Key_attr(Key_type::PK)));

    Parser::step = Step::create_table_comma_or_closing_parens;
}
