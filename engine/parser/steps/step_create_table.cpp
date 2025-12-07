#include "../Parser.hpp"
#include "../../utils/ut_is_identifier/ut_is_identifier.hpp"
#include "../../utils/ut_str_toupper/ut_str_toupper.hpp"

void Parser::step_create_table()
{
    auto is_table_name = Parser::peek_is_table_name("at CREATE: expected a Table name");
    if (!is_table_name) return;
    Parser::step = Step::create_table_opening_parens;
}

void Parser::step_create_table_opening_parens()
{
    auto opening_parens = Parser::peek();
    if (opening_parens != "(" || opening_parens.size() != 1)
    {
        Parser::step = Step::error;
        Parser::error_message = "at CREATE: expected an opening parens";
        return;
    }
    Parser::pop();
    Parser::step = Step::create_table_field_name;
}

void Parser::step_create_table_field_name()
{
    auto identifier = Parser::peek();
    //    ! in case of an alternative for the primary key !
    //    if(identifier == "PRIMARY KEY") {
    //        Parser::Step = Step::primary_key_opening_parens;
    //        Parser::pop();
    //        return;
    //    }
    if (!is_identifier(identifier))
    {
        Parser::step = Step::error;
        Parser::error_message = "at CREATE: expected a Field name";
        return;
    }
    Parser::current_create_table_field_val = identifier;
    Parser::pop();
    Parser::step = Step::create_table_field_type;
}

void Parser::step_create_table_field_type() {
    auto type = Parser::peek();
    if (type.empty() or type == ",") {
        Parser::step = Step::error;
        Parser::error_message = "at CREATE: expected a type of the Field";
        return;
    }
    Data_type d_type = return_data_type_by_str(type);
    if (d_type == Data_type::UNKNOWN) {
        Parser::step = Step::error;
        Parser::error_message = "at CREATE: data type provided for the Field is invalid";
        return;
    }
    Parser::current_field_data_type = d_type;
    for (auto &field: Parser::q.get_fields()) {
        auto name = field.value;
        auto current_field_name = Parser::current_create_table_field_val;
        str_toupper(name);
        str_toupper(current_field_name);
        if (current_field_name == name) {
            Parser::step = Step::error;
            Parser::error_message = "at CREATE: Field with this name already exists";
            return;
        }
    }

    Parser::pop();

    auto next_peek = Parser::peek();
    str_toupper(next_peek);

    if (next_peek == "PRIMARY") {
        Parser::step = Step::primary_key_as_type;
        return;
    }
    if (next_peek == "FOREIGN") {
        Parser::step = Step::foreign_key_as_type;
        return;
    }

    Parser::q.append_field(Field(current_create_table_field_val, d_type));
    Parser::step = Step::create_table_comma_or_closing_parens;
}

void Parser::step_create_table_comma_or_closing_parens()
{
    auto comma_or_closing_paren = Parser::peek();
    if (comma_or_closing_paren != "," and comma_or_closing_paren != ")")
    {
        Parser::step = Step::error;
        Parser::error_message = "at CREATE: expected a comma or a closing parens";
        return;
    }
    Parser::pop();
    if (comma_or_closing_paren == ",")
    {
        Parser::step = Step::create_table_field_name;
        return;
    }
    // nothing is think because you can`t do shit after this q...
    Parser::pop_flag = true;
}
