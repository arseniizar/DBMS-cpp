//
// Created by Altezza on 14.05.2024.
//

#ifndef DATABASEPROJECT_PARSER_STEPS_HPP
#define DATABASEPROJECT_PARSER_STEPS_HPP

#include "parser_complementary.hpp"
#include "parser.hpp"

auto parser::step_type() {
    auto peeked = parser::peek();
    str_toupper(peeked);
    if (peeked == "SELECT") {
        parser::q.set_type(query_type::Select);
        parser::pop();
        parser::step = step::select_field;
    } else if (peeked == "INSERT INTO") {
        parser::q.set_type(query_type::Insert);
        parser::pop();
        parser::step = step::insert_table;
    } else if (peeked == "UPDATE") {
        parser::q.set_type(query_type::Update);
        parser::pop();
        parser::step = step::update_table;
    } else if (peeked == "DELETE FROM") {
        parser::q.set_type(query_type::Delete);
        parser::pop();
        parser::step = step::delete_from_table;
    } else if (peeked == "CREATE TABLE") {
        parser::q.set_type(query_type::Create);
        parser::pop();
        parser::step = step::create_table;
    } else {
        parser::step = step::error;
        parser::error_message = "Invalid query type";
        return;
    }
}

auto parser::step_select_field() {
    auto identifier = parser::peek();
    if (!is_identifier_or_asterisk(identifier)) {
        parser::step = step::error;
        parser::error_message = "at SELECT: expected a field to select";
        return;
    }
    parser::q.append_field(identifier, data_type::TABLE_SELECT);
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

auto parser::step_select_comma() {
    auto isComma = parser::peek_is_comma("at SELECT: expected a comma after the field to select");
    if (!isComma) return;
    parser::step = step::select_field;
}

auto parser::step_select_from() {
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

auto parser::step_select_from_table() {
    auto table_name = parser::peek();
    if (table_name.empty()) {
        parser::step = step::error;
        parser::error_message = "at SELECT: expected a name of the table";
        return;
    }
    parser::q.set_table_name(table_name);
    parser::pop();
    if(is_peek_empty()) return;
    parser::step = step::where;
}

auto parser::step_insert_table() {
    auto table_name = parser::peek();
    if (table_name.empty() or !is_identifier(table_name)) {
        parser::step = step::error;
        parser::error_message = "at INSERT: expected a table name";
        return;
    }
    parser::q.set_table_name(table_name);
    parser::pop();
    parser::step = step::insert_fields_opening_parens;
}

auto parser::step_insert_fields_opening_parens() {
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

auto parser::step_insert_fields() {
    auto identifier = parser::peek();
    if (!is_identifier(identifier)) {
        parser::step = step::error;
        parser::error_message = "at INSERT: expected a field to insert";
        return;
    }
    data_type d_type = data_type::INSERT_COL;
    parser::q.append_field(identifier, d_type);
    parser::pop();
    parser::step = step::insert_fields_comma_or_closing_parens;
}

auto parser::step_insert_fields_comma_or_closing_parens() {
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

auto parser::step_insert_values_opening_parens() {
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

auto parser::step_insert_values_rword() {
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

auto parser::step_insert_values() {
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

auto parser::step_insert_values_comma_or_closing_parens() {
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
    if (curr_insert_row.size() < parser::q.get_fields_size()) {
        parser::step = step::error;
        parser::error_message = "at INSERT: expected a group of inserts";
        return;
    }
    parser::pop();
    if(is_peek_empty()) return;
    parser::step = step::insert_values_comma_before_opening_parens;
}

auto parser::step_insert_values_comma_before_opening_parens() {
    auto isComma = parser::peek_is_comma("at INSERT: expected a comma before opening parens");
    if (!isComma) return;
    parser::step = step::insert_values_opening_parens;
}

auto parser::step_update_table() {
    auto table_name = parser::peek();
    if (table_name.empty()) {
        parser::step = step::error;
        parser::error_message = "at UPDATE: expected a table name";
        return;
    }
    parser::q.set_table_name(table_name);
    parser::pop();
    parser::step = step::update_set;
}

auto parser::step_update_set() {
    auto set = parser::peek();
    if (set != "SET") {
        parser::step = step::error;
        parser::error_message = "at UPDATE: expected keyword SET";
        return;
    }
    parser::pop();
    parser::step = step::update_field;
}

auto parser::step_update_field() {
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

auto parser::step_update_equals() {
    auto equals = parser::peek();
    if (equals != "=") {
        parser::step = step::error;
        parser::error_message = "at UPDATE: expected an '='";
        return;
    }
    parser::pop();
    parser::step = step::update_value;
}

auto parser::step_update_value() {
    auto pair = parser::peek_quoted_with_length();
    if (pair.second == 0) {
        parser::step = step::error;
        parser::error_message = "at UPDATE: expected a value to update with";
        return;
    }
    parser::q.append_update(parser::next_update_field, pair.first);
    parser::next_update_field.erase();
    parser::pop();
    auto where = parser::peek();
    str_toupper(where);
    if (where == "WHERE") {
        parser::step = step::where;
        return;
    }
    if(is_peek_empty()) return;
    parser::step = step::update_comma;
}

auto parser::step_update_comma() {
    auto isComma = parser::peek_is_comma("at UPDATE: expected a comma");
    if (!isComma) return;
    parser::step = step::update_field;
}

auto parser::step_delete_from_table() {
    auto table_name = parser::peek();
    if (table_name.empty()) {
        parser::step = step::error;
        parser::error_message = "at DELETE: expected the name of the table";
        return;
    }
    parser::q.set_table_name(table_name);
    parser::pop();
    if(is_peek_empty()) return;
    parser::step = step::where;
}

auto parser::step_where() {
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

auto parser::step_where_field() {
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

auto parser::step_where_operator() {
    auto op = parser::peek();
    auto curr_cond = parser::q.get_current_condition();
    if (op == "=") curr_cond.query_operator = query_operator::eq;
    else if (op == ">") curr_cond.query_operator = query_operator::gt;
    else if (op == ">=") curr_cond.query_operator = query_operator::gte;
    else if (op == "<") curr_cond.query_operator = query_operator::lt;
    else if (op == "<=") curr_cond.query_operator = query_operator::lte;
    else if (op == "!=") curr_cond.query_operator = query_operator::ne;
    else {
        parser::step = step::error;
        parser::error_message = "at WHERE: expected a WHERE operator";
        return;
    }
    parser::q.set_current_condition(curr_cond);
    parser::pop();
    parser::step = step::where_value;
}

auto parser::step_where_value() {
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

auto parser::step_where_and() {
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

auto parser::step_create_table() {
    auto table_name = parser::peek();
    if (table_name.empty() || table_name == "(") {
        parser::step = step::error;
        parser::error_message = "at CREATE: expected a table name";
        return;
    }
    parser::q.set_table_name(table_name);
    parser::pop();
    parser::step = step::create_table_opening_parens;
}

auto parser::step_create_table_opening_parens() {
    auto opening_parens = parser::peek();
    if (opening_parens != "(" || opening_parens.size() != 1) {
        parser::step = step::error;
        parser::error_message = "at CREATE: expected an opening parens";
        return;
    }
    parser::pop();
    parser::step = step::create_table_field_name;
}

auto parser::step_create_table_field_name() {
    auto identifier = parser::peek();
    if (!is_identifier(identifier)) {
        parser::step = step::error;
        parser::error_message = "at CREATE: expected a field name";
        return;
    }
    parser::current_create_table_field = identifier;
    parser::pop();
    parser::step = step::create_table_field_type;
}

auto parser::step_create_table_field_type() {
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
    for (auto &field: parser::q.get_fields()) {
        auto name = field.first;
        auto current_field_name = parser::current_create_table_field;
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
    parser::q.append_field(current_create_table_field, d_type);
    parser::pop();
    parser::step = step::create_table_comma_or_closing_parens;
}

auto parser::step_create_table_comma_or_closing_parens() {
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



auto parser::step_error() {
    return parser::make_error_pair(parser::error_message);
}

auto parser::step_group_by() {
    return nullptr;
}

auto parser::step_group_by_field() {
    return nullptr;
}

auto parser::step_having() {
    return nullptr;
}

auto parser::step_having_field() {
    return nullptr;
}

auto parser::step_having_operator() {
    return nullptr;
}

auto parser::step_having_value() {
    return nullptr;
}

auto parser::step_join() {
    return nullptr;
}

auto parser::step_join_table() {
    return nullptr;
}

auto parser::step_join_on() {
    return nullptr;
}

auto parser::step_join_on_field1() {
    return nullptr;
}

auto parser::step_join_on_equal() {
    return nullptr;
}


#endif //DATABASEPROJECT_PARSER_STEPS_HPP
