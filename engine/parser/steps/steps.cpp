//
// Created by Altezza on 17.05.2024.
//
#include "../parser.hpp"
#include "../../utils/ut_is_identifier/ut_is_identifier.hpp"
#include "../../utils/ut_str_toupper/ut_str_toupper.hpp"

void parser::step_type() {
    auto peeked = parser::peek();
    str_toupper(peeked);
    if (peeked == "SELECT") {
        parser::q.set_query_type(query_type::Select);
        parser::pop();
        parser::step = step::select_field;
    } else if (peeked == "INSERT INTO") {
        parser::q.set_query_type(query_type::Insert);
        parser::pop();
        parser::step = step::insert_table;
    } else if (peeked == "UPDATE") {
        parser::q.set_query_type(query_type::Update);
        parser::pop();
        parser::step = step::update_table;
    } else if (peeked == "DELETE FROM") {
        parser::q.set_query_type(query_type::Delete);
        parser::pop();
        parser::step = step::delete_from_table;
    } else if (peeked == "CREATE TABLE") {
        parser::q.set_query_type(query_type::Create);
        parser::pop();
        parser::step = step::create_table;
    } else {
        parser::step = step::error;
        parser::error_message = "Invalid query type";
        return;
    }
}

void parser::step_select_field() {
    auto identifier = parser::peek();
    if (!is_identifier_or_asterisk(identifier)) {
        parser::step = step::error;
        parser::error_message = "at SELECT: expected a field to select";
        return;
    }
    parser::q.append_field(field(identifier, data_type::TABLE_SELECT));
    parser::pop();
    auto from = parser::peek();
    str_toupper(from);
    if (from == "AS") {
        parser::pop();
        auto alias = parser::peek();
        if (!is_identifier(alias)) {
            parser::step = step::error;
            parser::error_message = "at SELECT: expected an alias after AS";
            return;
        }
        parser::q.append_alias(identifier, alias);
        parser::pop();
        from = parser::peek();
    }
    str_toupper(from);
    if (from == "FROM") {
        parser::step = step::select_from;
    } else {
        parser::step = step::select_comma;
    }
}

void parser::step_select_comma() {
    auto isComma = parser::peek_is_comma("at SELECT: expected a comma after the field to select");
    if (!isComma) return;
    parser::step = step::select_field;
}

void parser::step_select_from() {
    auto from = parser::peek();
    str_toupper(from);
    if (from != "FROM") {
        parser::step = step::error;
        parser::error_message = "at SELECT: expected a keyword FROM before the name of the table";
        return;
    }
    parser::pop();
    parser::step = step::select_from_table;
}

void parser::step_select_from_table() {
    auto is_table_name = parser::peek_is_table_name("at SELECT: expected a name of the table");
    if (!is_table_name) return;
    parser::set_additional_step();
}

void parser::step_insert_table() {
    auto is_table_name = parser::peek_is_table_name("at INSERT: expected a table name");
    if (!is_table_name) return;
    parser::step = step::insert_fields_opening_parens;
}

void parser::step_insert_fields_opening_parens() {
    auto opening_parens = parser::peek();
    if (opening_parens == "VALUES") {
        parser::step = step::insert_values_rword;
        return;
    }
    if (opening_parens != "(" || opening_parens.size() != 1) {
        parser::step = step::error;
        parser::error_message = "at INSERT: expected an opening parens";
        return;
    }
    parser::pop();
    parser::step = step::insert_fields;
}

void parser::step_insert_fields() {
    auto identifier = parser::peek();
    if (!is_identifier(identifier)) {
        parser::step = step::error;
        parser::error_message = "at INSERT: expected a field to insert";
        return;
    }
    parser::q.append_field(field(identifier, data_type::INSERT_COL));
    parser::pop();
    parser::step = step::insert_fields_comma_or_closing_parens;
}

void parser::step_insert_fields_comma_or_closing_parens() {
    auto comma_or_closing_paren = parser::peek();
    if (comma_or_closing_paren != "," and comma_or_closing_paren != ")") {
        parser::step = step::error;
        parser::error_message = "at INSERT: expected comma or closing parens";
        return;
    }
    parser::pop();
    if (comma_or_closing_paren == ",") {
        parser::step = step::insert_fields;
        return;
    }
    parser::step = step::insert_values_rword;
}

void parser::step_insert_values_opening_parens() {
    auto opening_parens = parser::peek();
    if (opening_parens != "(") {
        parser::step = step::error;
        parser::error_message = "at INSERT: expected an opening parens";
        return;
    }
    parser::q.append_inserts_vec(std::vector<std::string>());
    parser::pop();
    parser::step = step::insert_values;
}

void parser::step_insert_values_rword() {
    auto values = parser::peek();
    str_toupper(values);
    if (values != "VALUES") {
        parser::step = step::error;
        parser::error_message = "at INSERT: expected keyword VALUES";
        return;
    }
    parser::pop();
    parser::step = step::insert_values_opening_parens;
}

void parser::step_insert_values() {
    auto pair = parser::peek_quoted_with_length();
    if (pair.second == 0) {
        parser::step = step::error;
        parser::error_message = "at INSERT: expected a value to insert";
        return;
    }
    parser::q.append_insert(pair.first);
    parser::pop();
    parser::step = step::insert_values_comma_or_closing_parens;
}

void parser::step_insert_values_comma_or_closing_parens() {
    auto comma_or_closing_parens = parser::peek();
    if (comma_or_closing_parens != "," and comma_or_closing_parens != ")") {
        parser::step = step::error;
        parser::error_message = "at INSERT: expected a comma or a closing parens";
        return;
    }
    parser::pop();
    if (comma_or_closing_parens == ",") {
        parser::step = step::insert_values;
        return;
    }
    auto curr_insert_row = parser::q.get_current_inserts();
    if (curr_insert_row.size() < parser::q.get_inserts_size()) {
        parser::step = step::error;
        parser::error_message = "at INSERT: expected a group of inserts";
        return;
    }
    parser::pop();
    if (is_peek_empty()) parser::pop_flag = true;
    parser::step = step::insert_values_comma_before_opening_parens;
}

void parser::step_insert_values_comma_before_opening_parens() {
    auto isComma = parser::peek_is_comma("at INSERT: expected a comma before opening parens");
    if (!isComma) return;
    parser::step = step::insert_values_opening_parens;
}

void parser::step_update_table() {
    auto is_table_name = parser::peek_is_table_name("at UPDATE: expected a table name");
    if (!is_table_name) return;
    parser::step = step::update_set;
}

void parser::step_update_set() {
    auto set = parser::peek();
    if (set != "SET") {
        parser::step = step::error;
        parser::error_message = "at UPDATE: expected keyword SET";
        return;
    }
    parser::pop();
    parser::step = step::update_field;
}

void parser::step_update_field() {
    auto identifier = parser::peek();
    if (!is_identifier(identifier)) {
        parser::step = step::error;
        parser::error_message = "at UPDATE: expected a field to update";
        return;
    }
    parser::next_update_field = parser::peek();
    parser::pop();
    parser::step = step::update_equals;
}

void parser::step_update_equals() {
    auto equals = parser::peek();
    if (equals != "=") {
        parser::step = step::error;
        parser::error_message = "at UPDATE: expected an '='";
        return;
    }
    parser::pop();
    parser::step = step::update_value;
}

void parser::step_update_value() {
    auto pair = parser::peek_quoted_with_length();
    if (pair.second == 0) {
        parser::step = step::error;
        parser::error_message = "at UPDATE: expected a value to update with";
        return;
    }
    parser::q.append_update(parser::next_update_field, pair.first);
    parser::next_update_field.erase();
    parser::pop();
    if (is_peek_empty()) parser::pop_flag = true;
    else parser::set_additional_step();
}

void parser::step_update_comma() {
    auto isComma = parser::peek_is_comma("at UPDATE: expected a comma");
    if (!isComma) return;
    parser::step = step::update_field;
}

void parser::step_delete_from_table() {
    auto is_table_name = parser::peek_is_table_name("at DELETE: expected the name of the table");
    if (!is_table_name) return;
    parser::step = step::where;
}

void parser::step_where() {
    auto where = parser::peek();
    str_toupper(where);
    if (where != "WHERE") {
        parser::step = step::error;
        parser::error_message = "expected a keyword WHERE after the end of the previous statement";
        return;
    }
    parser::pop();
    parser::step = step::where_field;
}

void parser::step_where_field() {
    auto identifier = parser::peek();
    if (!is_identifier(identifier)) {
        parser::step = step::error;
        parser::error_message = "at WHERE: expected a field";
        return;
    }
    parser::q.append_condition(condition(identifier, true));
    parser::pop();
    parser::step = step::where_operator;
}

void parser::step_where_operator() {
    auto is_op = peek_is_operator("at WHERE: expected a WHERE operator");
    if (!is_op) return;
    parser::step = step::where_value;
}

void parser::step_where_value() {
    auto curr_cond = parser::q.get_current_condition();
    auto identifier = parser::peek();
    if (is_identifier(identifier)) {
        curr_cond.operand2 = identifier;
        curr_cond.operand2_is_field = true;
    } else {
        auto pair = parser::peek_quoted_with_length();
        if (pair.second == 0) {
            parser::step = step::error;
            parser::error_message = "at WHERE: expected a value";
            return;
        }
        curr_cond.operand2 = pair.first;
        curr_cond.operand2_is_field = false;
    }
    parser::q.set_current_condition(curr_cond);
    parser::pop();
    parser::step = step::where_and;
}

void parser::step_where_and() {
    auto and_word = parser::peek();
    str_toupper(and_word);
    if (and_word != "AND") {
        parser::step = step::error;
        parser::error_message = "at WHERE: expected a keyword AND";
        return;
    }
    parser::pop();
    parser::step = step::where_field;
}

void parser::step_create_table() {
    auto is_table_name = parser::peek_is_table_name("at CREATE: expected a table name");
    if (!is_table_name) return;
    parser::step = step::create_table_opening_parens;
}

void parser::step_create_table_opening_parens() {
    auto opening_parens = parser::peek();
    if (opening_parens != "(" || opening_parens.size() != 1) {
        parser::step = step::error;
        parser::error_message = "at CREATE: expected an opening parens";
        return;
    }
    parser::pop();
    parser::step = step::create_table_field_name;
}

void parser::step_create_table_field_name() {
    auto identifier = parser::peek();
//    ! in case of an alternative for the primary key !
//    if(identifier == "PRIMARY KEY") {
//        parser::step = step::primary_key_opening_parens;
//        parser::pop();
//        return;
//    }
    if (!is_identifier(identifier)) {
        parser::step = step::error;
        parser::error_message = "at CREATE: expected a field name";
        return;
    }
    parser::current_create_table_field_val = identifier;
    parser::pop();
    parser::step = step::create_table_field_type;
}

void parser::step_create_table_field_type() {
    auto type = parser::peek();
    if (type.empty() or type == ",") {
        parser::step = step::error;
        parser::error_message = "at CREATE: expected a type of the field";
        return;
    }
    data_type d_type = return_data_type_by_str(type);
    if (d_type == data_type::UNKNOWN) {
        parser::step = step::error;
        parser::error_message = "at CREATE: data type provided for the field is invalid";
        return;
    }
    parser::current_field_data_type = d_type;
    for (auto &field: parser::q.get_fields()) {
        auto name = field.value;
        auto current_field_name = parser::current_create_table_field_val;
        str_toupper(name);
        str_toupper(current_field_name);
        if (current_field_name == name) {
            parser::step = step::error;
            parser::error_message = "at CREATE: field with this name already exists";
            return;
        }
    }
    // maybe I should add field in the upper case, or IDK, because in IDE like
    // DATAGRIP it works even with lowercase names of the tables
    parser::pop();
    auto next_peek = parser::peek();
    if (next_peek == "PRIMARY KEY") {
        parser::step = step::primary_key_as_type;
        return;
    }
    if (next_peek == "FOREIGN KEY") {
        parser::step = step::foreign_key_as_type;
        return;
    }
    parser::q.append_field(field(current_create_table_field_val, d_type));
    parser::step = step::create_table_comma_or_closing_parens;
}

void parser::step_create_table_comma_or_closing_parens() {
    auto comma_or_closing_paren = parser::peek();
    if (comma_or_closing_paren != "," and comma_or_closing_paren != ")") {
        parser::step = step::error;
        parser::error_message = "at CREATE: expected a comma or a closing parens";
        return;
    }
    parser::pop();
    if (comma_or_closing_paren == ",") {
        parser::step = step::create_table_field_name;
        return;
    }
    // nothing is think because you can`t do shit after this q...
    parser::pop_flag = true;
}

void parser::step_group_by() {
    auto peeked = parser::peek();
    str_toupper(peeked);
    if (peeked != "GROUP BY") {
        parser::step = step::error;
        parser::error_message = "at GROUP BY: expected a GROUP BY";
        return;
    }
    parser::pop();
    parser::step = step::group_by_field;
}

void parser::step_group_by_field() {
    auto identifier = parser::peek();
    str_toupper(identifier);
    std::string current_select_field_val = parser::q.get_current_select_field().value;
    if (!is_identifier(identifier) or identifier == current_select_field_val) {
        parser::step = step::error;
        parser::error_message = "at GROUP BY: expected a field to group by";
        return;
    }
    parser::pop();
    if (is_peek_empty()) parser::pop_flag = true;
    parser::step = step::having;
}

void parser::step_having() {
    auto peeked = parser::peek();
    str_toupper(peeked);
    if (peeked != "HAVING") {
        parser::step = step::error;
        parser::error_message = "expected a keyword HAVING after GROUP BY";
    }
    parser::pop();
    parser::step = step::having_field;
}

void parser::step_having_field() {
    auto identifier = parser::peek();
    if (!is_identifier(identifier)) {
        parser::step = step::error;
        parser::error_message = "at HAVING: expected a field to select";
        return;
    }
    parser::pop();
    parser::step = step::having_operator;
}

void parser::step_having_operator() {
    auto is_op = peek_is_operator("at HAVING: expected an operator");
    if (!is_op) return;
    parser::step = step::having_value;
}

void parser::step_having_value() {
    auto identifier = parser::peek();
    if (!is_identifier(identifier)) {
        parser::step = step::error;
        parser::error_message = "at HAVING: expected a value";
        return;
    }
    parser::pop();
    parser::pop_flag = true;
}

void parser::step_join() {
    auto join = parser::peek();
    if (join != "JOIN") {
        parser::step = step::error;
        parser::error_message = "expected a JOIN after the end of the previous statement";
        return;
    }
    str_toupper(join);
    auto join_type_str = return_join_type_str(join);
    join_type j_t = return_join_type(join_type_str);
    parser::q.set_join_type(j_t);
    parser::pop();
    parser::step = step::join_table;
}

void parser::step_join_table() {
    auto is_table_name = parser::peek_is_table_name("at JOIN: expected a table name");
    if (!is_table_name) return;
    parser::step = step::join_on;
}

void parser::step_join_on() {
    auto on = parser::peek();
    if (on != "ON") {
        parser::step = step::error;
        parser::error_message = "at JOIN: expected a keyword ON";
        return;
    }
    parser::pop();
    parser::step = step::join_on_field1;
}

void parser::step_join_on_field1() {
    auto identifier = parser::peek();
    if (!is_identifier(identifier)) {
        parser::step = step::error;
        parser::error_message = "at JOIN: expected a field";
        return;
    }
    parser::q.append_condition(condition(identifier, true));
    parser::pop();
    parser::step = step::join_on_equal;
}

void parser::step_join_on_equal() {
    auto is_op = peek_is_operator("at WHERE: expected a WHERE operator");
    if (!is_op) return;
    parser::step = step::join_on_field2;
}

void parser::step_join_on_field2() {
    auto identifier = parser::peek();
    auto curr_cond = parser::q.get_current_condition();
    if (!is_identifier(identifier)) {
        parser::step = step::error;
        parser::error_message = "at JOIN: expected a field";
        return;
    }
    curr_cond.operand2 = identifier;
    curr_cond.operand2_is_field = true;
    parser::q.set_current_condition(curr_cond);
    parser::pop();
    parser::pop_flag = true;
}

void parser::step_primary_key_as_type() {
    auto primary_key = parser::peek();
    if (primary_key != "PRIMARY KEY") {
        parser::step = step::error;
        parser::error_message = "expected a keyword PRIMARY KEY after the type of the field";
        return;
    }
    parser::q.append_field(field(parser::current_create_table_field_val,
                                 parser::current_field_data_type,
                                 key_attr(key_type::PK)));
    parser::pop();
    parser::step = step::create_table_comma_or_closing_parens;
}

void parser::step_primary_key_at_end() {
    auto primary_key = parser::peek();
    if (primary_key != "PRIMARY KEY") {
        parser::step = step::error;
        parser::error_message = "expected a keyword PRIMARY KEY after the type of the field";
        return;
    }
    parser::pop();
    parser::step = step::primary_key_opening_parens;
}

void parser::step_primary_key_opening_parens() {
    auto is_opening_parens =
            peek_is_opening_parens("expected an opening parens after the PRIMARY KEY keyword");
    if (!is_opening_parens) return;
    parser::step = step::primary_key_field;
}

void parser::step_primary_key_field() {
    auto identifier = parser::peek();
    if (!is_identifier(identifier)) {
        parser::step = step::error;
        parser::error_message = "expected a proper name of one of the fields from the table";
        return;
    }
    bool is_field_present = std::any_of(parser::q.get_fields().begin(),
                                        parser::q.get_fields().end(),
                                        [&identifier](field const &field) {
                                            return field.value == identifier;
                                        });
    if (!is_field_present) {
        parser::step = step::error;
        parser::error_message = "field with the name " + identifier + " doesn't exist in this table";
        return;
    }
    auto field_iter = get_field_by_name(identifier);
    field_iter->k_a = key_attr(key_type::PK);
    parser::pop();
    parser::step = step::primary_key_comma_or_closing_parens;
}

void parser::step_primary_key_comma_or_closing_parens() {
    auto comma_or_closing_parens = parser::peek();
    if (comma_or_closing_parens != "," and comma_or_closing_parens != ")") {
        parser::step = step::error;
        parser::error_message = "expected a closing parens or comma after the field inside PRIMARY KEY()";
        return;
    }
    if (comma_or_closing_parens == ",") {
        parser::step = step::primary_key_field;
        return;
    }
    parser::pop_flag = true;
    parser::step = step::create_table_comma_or_closing_parens;
}

void parser::step_foreign_key() {
    auto foreign_key = parser::peek();
    if (foreign_key != "FOREIGN KEY") {
        parser::step = step::error;
        parser::error_message = "expected a FOREIGN KEY keyword!";
        return;
    }
    parser::pop();
    parser::step = step::foreign_key_opening_parens;
}

void parser::step_foreign_key_as_type() {
    auto foreign_key = parser::peek();
    if (foreign_key != "FOREIGN KEY") {
        parser::step = step::error;
        parser::error_message = "expected a keyword FOREIGN KEY after the type of the field";
        return;
    }
    parser::q.append_field(field(parser::current_create_table_field_val,
                                 parser::current_field_data_type,
                                 key_attr(key_type::FK)));
    parser::pop();
    parser::step = step::references;
}

void parser::step_references() {
    auto references = parser::peek();
    if (references != "REFERENCES") {
        parser::step = step::error;
        parser::error_message = "expected a keyword REFERENCES after the FOREIGN KEY keyword";
        return;
    }
    parser::pop();
    parser::step = step::references_table;
}

void parser::step_references_table() {
    auto is_table_name = peek_is_table_name("at REFERENCES: expected the name a the table");
    if (!is_table_name) return;
    parser::step = step::references_table_opening_parens;
}

void parser::step_references_table_opening_parens() {
    auto is_opening_parens = parser::peek_is_opening_parens(
            "at REFERENCES: expected an opening parens after the table name");
    if (!is_opening_parens) return;
    parser::step = step::references_field;
}

void parser::step_references_field() {
    auto identifier = parser::peek();
    if (!is_identifier(identifier)) {
        parser::step = step::error;
        parser::error_message = "at REFERENCES: expected a proper name for the the field";
        return;
    }
    // I will check this referencing fields in the dbms before putting it into the executor
    parser::q.append_referencing_field_name(identifier);
    parser::pop();
    parser::step = step::references_comma_or_closing_parens;
}

void parser::step_references_comma_or_closing_parens() {
    auto comma_or_closing_parens = parser::peek();
    if (comma_or_closing_parens != "," and comma_or_closing_parens != ")") {
        parser::step = step::error;
        parser::error_message = "at REFERENCES: expected a comma or a closing parens";
    }
    auto current_field = parser::q.get_current_field();
    current_field.k_a.add_ref(ref(parser::q.get_referenced_table_name(),
                                  parser::q.get_referencing_fields_names().back()));
    parser::q.set_current_field(current_field);
    if (comma_or_closing_parens == ",") {
        parser::step = step::references_field;
        return;
    }
    parser::pop_flag = true;
    parser::step = step::create_table_comma_or_closing_parens;
    parser::pop();
}

void parser::step_foreign_key_opening_parens() {
    auto is_opening_parens = parser::peek_is_opening_parens(
            "at FOREIGN KEY: expected an opening parens");
    if (!is_opening_parens) return;
    parser::step = step::foreign_key_field;
}

void parser::step_foreign_key_field() {
    auto identifier = parser::peek();
    if (!is_identifier(identifier)) {
        parser::step = step::error;
        parser::error_message = "at FOREIGN KEY: expected a field that will be a foreign key";
        return;
    }
    auto f = std::find_if(
            parser::q.get_fields().begin(),
            parser::q.get_fields().end(),
            [&identifier](field const &f) {
                return f.value == identifier;
            }
    );
    f->k_a = key_attr(key_type::FK, ref(current_references_table_name,
                                        f->value));
    parser::pop();
    parser::step = step::foreign_key_comma_or_closing_parens;
}

void parser::step_foreign_key_comma_or_closing_parens() {
    auto comma_or_closing_parens = parser::peek();
    if (comma_or_closing_parens != "," and comma_or_closing_parens != ")") {
        parser::step = step::error;
        parser::error_message = "at FOREIGN KEY: expected a comma or a closing parens";
        return;
    }
    if (comma_or_closing_parens == ",") {
        parser::step = step::foreign_key_field;
        return;
    }
    parser::pop();
    parser::step = step::references;
}

std::pair<query, parse_error> parser::step_error() {
    return parser::make_error_pair(parser::error_message);
}
