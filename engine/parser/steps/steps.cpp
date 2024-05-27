//
// Created by Altezza on 17.05.2024.
//
#include "../Parser.hpp"
#include "../../utils/ut_is_identifier/ut_is_identifier.hpp"
#include "../../utils/ut_str_toupper/ut_str_toupper.hpp"

void Parser::step_type() {
    auto peeked = Parser::peek();
    str_toupper(peeked);
    if (peeked == "SELECT") {
        Parser::q.set_query_type(query_type::Select);
        Parser::pop();
        Parser::step = Step::select_field;
    } else if (peeked == "INSERT INTO") {
        Parser::q.set_query_type(query_type::Insert);
        Parser::pop();
        Parser::step = Step::insert_table;
    } else if (peeked == "UPDATE") {
        Parser::q.set_query_type(query_type::Update);
        Parser::pop();
        Parser::step = Step::update_table;
    } else if (peeked == "DELETE FROM") {
        Parser::q.set_query_type(query_type::Delete);
        Parser::pop();
        Parser::step = Step::delete_from_table;
    } else if (peeked == "CREATE TABLE") {
        Parser::q.set_query_type(query_type::Create);
        Parser::pop();
        Parser::step = Step::create_table;
    } else {
        Parser::step = Step::error;
        Parser::error_message = "Invalid Query type";
        return;
    }
}

void Parser::step_select_field() {
    auto identifier = Parser::peek();
    if (!is_identifier_or_asterisk(identifier)) {
        Parser::step = Step::error;
        Parser::error_message = "at SELECT: expected a Field to select";
        return;
    }
    Parser::q.append_field(Field(identifier, Data_type::TABLE_SELECT));
    Parser::pop();
    auto from = Parser::peek();
    str_toupper(from);
    if (from == "AS") {
        Parser::pop();
        auto alias = Parser::peek();
        if (!is_identifier(alias)) {
            Parser::step = Step::error;
            Parser::error_message = "at SELECT: expected an alias after AS";
            return;
        }
        Parser::q.append_alias(identifier, alias);
        Parser::pop();
        from = Parser::peek();
    }
    str_toupper(from);
    if (from == "FROM") {
        Parser::step = Step::select_from;
    } else {
        Parser::step = Step::select_comma;
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
    auto is_table_name = Parser::peek_is_table_name("at SELECT: expected a name of the Table");
    if (!is_table_name) return;
    Parser::set_additional_step();
}

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

void Parser::step_update_table() {
    auto is_table_name = Parser::peek_is_table_name("at UPDATE: expected a Table name");
    if (!is_table_name) return;
    Parser::step = Step::update_set;
}

void Parser::step_update_set() {
    auto set = Parser::peek();
    if (set != "SET") {
        Parser::step = Step::error;
        Parser::error_message = "at UPDATE: expected keyword SET";
        return;
    }
    Parser::pop();
    Parser::step = Step::update_field;
}

void Parser::step_update_field() {
    auto identifier = Parser::peek();
    if (!is_identifier(identifier)) {
        Parser::step = Step::error;
        Parser::error_message = "at UPDATE: expected a Field to update";
        return;
    }
    Parser::next_update_field = Parser::peek();
    Parser::pop();
    Parser::step = Step::update_equals;
}

void Parser::step_update_equals() {
    auto equals = Parser::peek();
    if (equals != "=") {
        Parser::step = Step::error;
        Parser::error_message = "at UPDATE: expected an '='";
        return;
    }
    Parser::pop();
    Parser::step = Step::update_value;
}

void Parser::step_update_value() {
    auto pair = Parser::peek_quoted_with_length();
    if (pair.second == 0) {
        Parser::step = Step::error;
        Parser::error_message = "at UPDATE: expected a value to update with";
        return;
    }
    Parser::q.append_update(Parser::next_update_field, pair.first);
    Parser::next_update_field.erase();
    Parser::pop();
    if (is_peek_empty()) Parser::pop_flag = true;
    else Parser::set_additional_step();
}

void Parser::step_update_comma() {
    auto isComma = Parser::peek_is_comma("at UPDATE: expected a comma");
    if (!isComma) return;
    Parser::step = Step::update_field;
}

void Parser::step_delete_from_table() {
    auto is_table_name = Parser::peek_is_table_name("at DELETE: expected the name of the Table");
    if (!is_table_name) return;
    Parser::step = Step::where;
}

void Parser::step_where() {
    auto where = Parser::peek();
    str_toupper(where);
    if (where != "WHERE") {
        Parser::step = Step::error;
        Parser::error_message = "expected a keyword WHERE after the end of the previous statement";
        return;
    }
    Parser::pop();
    Parser::step = Step::where_field;
}

void Parser::step_where_field() {
    auto identifier = Parser::peek();
    if (!is_identifier(identifier)) {
        Parser::step = Step::error;
        Parser::error_message = "at WHERE: expected a Field";
        return;
    }
    Parser::q.append_condition(Condition(identifier, true));
    Parser::pop();
    Parser::step = Step::where_operator;
}

void Parser::step_where_operator() {
    auto is_op = peek_is_operator("at WHERE: expected a WHERE operator");
    if (!is_op) return;
    Parser::step = Step::where_value;
}

void Parser::step_where_value() {
    auto curr_cond = Parser::q.get_current_condition();
    auto identifier = Parser::peek();
    if (is_identifier(identifier)) {
        curr_cond.operand2 = identifier;
        curr_cond.operand2_is_field = true;
    } else {
        auto pair = Parser::peek_quoted_with_length();
        if (pair.second == 0) {
            Parser::step = Step::error;
            Parser::error_message = "at WHERE: expected a value";
            return;
        }
        curr_cond.operand2 = pair.first;
        curr_cond.operand2_is_field = false;
    }
    Parser::q.set_current_condition(curr_cond);
    Parser::pop();
    Parser::step = Step::where_and;
}

void Parser::step_where_and() {
    auto and_word = Parser::peek();
    str_toupper(and_word);
    if (and_word != "AND") {
        Parser::step = Step::error;
        Parser::error_message = "at WHERE: expected a keyword AND";
        return;
    }
    Parser::pop();
    Parser::step = Step::where_field;
}

void Parser::step_create_table() {
    auto is_table_name = Parser::peek_is_table_name("at CREATE: expected a Table name");
    if (!is_table_name) return;
    Parser::step = Step::create_table_opening_parens;
}

void Parser::step_create_table_opening_parens() {
    auto opening_parens = Parser::peek();
    if (opening_parens != "(" || opening_parens.size() != 1) {
        Parser::step = Step::error;
        Parser::error_message = "at CREATE: expected an opening parens";
        return;
    }
    Parser::pop();
    Parser::step = Step::create_table_field_name;
}

void Parser::step_create_table_field_name() {
    auto identifier = Parser::peek();
//    ! in case of an alternative for the primary key !
//    if(identifier == "PRIMARY KEY") {
//        Parser::Step = Step::primary_key_opening_parens;
//        Parser::pop();
//        return;
//    }
    if (!is_identifier(identifier)) {
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
    // maybe I should add Field in the upper case, or IDK, because in IDE like
    // DATAGRIP it works even with lowercase names of the tables
    Parser::pop();
    auto next_peek = Parser::peek();
    if (next_peek == "PRIMARY KEY") {
        Parser::step = Step::primary_key_as_type;
        return;
    }
    if (next_peek == "FOREIGN KEY") {
        Parser::step = Step::foreign_key_as_type;
        return;
    }
    Parser::q.append_field(Field(current_create_table_field_val, d_type));
    Parser::step = Step::create_table_comma_or_closing_parens;
}

void Parser::step_create_table_comma_or_closing_parens() {
    auto comma_or_closing_paren = Parser::peek();
    if (comma_or_closing_paren != "," and comma_or_closing_paren != ")") {
        Parser::step = Step::error;
        Parser::error_message = "at CREATE: expected a comma or a closing parens";
        return;
    }
    Parser::pop();
    if (comma_or_closing_paren == ",") {
        Parser::step = Step::create_table_field_name;
        return;
    }
    // nothing is think because you can`t do shit after this q...
    Parser::pop_flag = true;
}

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

void Parser::step_having() {
    auto peeked = Parser::peek();
    str_toupper(peeked);
    if (peeked != "HAVING") {
        Parser::step = Step::error;
        Parser::error_message = "expected a keyword HAVING after GROUP BY";
    }
    Parser::pop();
    Parser::step = Step::having_field;
}

void Parser::step_having_field() {
    auto identifier = Parser::peek();
    if (!is_identifier(identifier)) {
        Parser::step = Step::error;
        Parser::error_message = "at HAVING: expected a Field to select";
        return;
    }
    Parser::pop();
    Parser::step = Step::having_operator;
}

void Parser::step_having_operator() {
    auto is_op = peek_is_operator("at HAVING: expected an operator");
    if (!is_op) return;
    Parser::step = Step::having_value;
}

void Parser::step_having_value() {
    auto identifier = Parser::peek();
    if (!is_identifier(identifier)) {
        Parser::step = Step::error;
        Parser::error_message = "at HAVING: expected a value";
        return;
    }
    Parser::pop();
    Parser::pop_flag = true;
}

void Parser::step_join() {
    auto join = Parser::peek();
    if (join != "JOIN") {
        Parser::step = Step::error;
        Parser::error_message = "expected a JOIN after the end of the previous statement";
        return;
    }
    str_toupper(join);
    auto join_type_str = return_join_type_str(join);
    join_type j_t = return_join_type(join_type_str);
    Parser::q.set_join_type(j_t);
    Parser::pop();
    Parser::step = Step::join_table;
}

void Parser::step_join_table() {
    auto is_table_name = Parser::peek_is_table_name("at JOIN: expected a Table name");
    if (!is_table_name) return;
    Parser::step = Step::join_on;
}

void Parser::step_join_on() {
    auto on = Parser::peek();
    if (on != "ON") {
        Parser::step = Step::error;
        Parser::error_message = "at JOIN: expected a keyword ON";
        return;
    }
    Parser::pop();
    Parser::step = Step::join_on_field1;
}

void Parser::step_join_on_field1() {
    auto identifier = Parser::peek();
    if (!is_identifier(identifier)) {
        Parser::step = Step::error;
        Parser::error_message = "at JOIN: expected a Field";
        return;
    }
    Parser::q.append_condition(Condition(identifier, true));
    Parser::pop();
    Parser::step = Step::join_on_equal;
}

void Parser::step_join_on_equal() {
    auto is_op = peek_is_operator("at WHERE: expected a WHERE operator");
    if (!is_op) return;
    Parser::step = Step::join_on_field2;
}

void Parser::step_join_on_field2() {
    auto identifier = Parser::peek();
    auto curr_cond = Parser::q.get_current_condition();
    if (!is_identifier(identifier)) {
        Parser::step = Step::error;
        Parser::error_message = "at JOIN: expected a Field";
        return;
    }
    curr_cond.operand2 = identifier;
    curr_cond.operand2_is_field = true;
    Parser::q.set_current_condition(curr_cond);
    Parser::pop();
    Parser::pop_flag = true;
}

void Parser::step_primary_key_as_type() {
    auto primary_key = Parser::peek();
    if (primary_key != "PRIMARY KEY") {
        Parser::step = Step::error;
        Parser::error_message = "expected a keyword PRIMARY KEY after the type of the Field";
        return;
    }
    auto potential_pk = Parser::q.get_primary_key();
    if(!potential_pk.empty()) {
        Parser::step = Step::error;
        Parser::error_message = "a table must have only one PRIMARY KEY";
        return;
    }
    Parser::q.append_field(Field(Parser::current_create_table_field_val,
                                 Parser::current_field_data_type,
                                 Key_attr(Key_type::PK)));
    Parser::pop();
    Parser::step = Step::create_table_comma_or_closing_parens;
}

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

std::pair<Query, Parse_error> Parser::step_error() {
    return Parser::make_error_pair(Parser::error_message);
}
