//
// Created by Altezza on 17.05.2024.
//
#include "../Parser.hpp"

std::pair<Query, Parse_error> Parser::parser_switch() {
    while (true) {
        if (is_index_at_end()) {
            Parser::q.set_command(Parser::sql);
            return std::make_pair(Parser::q, Parser::error);
        }
        switch (Parser::step) {
            case Step::type: {
                Parser::step_type();
                break;
            }
            case Step::select_field: {
                Parser::step_select_field();
                break;
            }
            case Step::select_comma: {
                Parser::step_select_comma();
                break;
            }
            case Step::select_from : {
                Parser::step_select_from();
                break;
            }
            case Step::select_from_table: {
                Parser::step_select_from_table();
                break;
            }
            case Step::insert_table: {
                Parser::step_insert_table();
                break;
            }
            case Step::delete_from_table: {
                Parser::step_delete_from_table();
                break;
            }
            case Step::create_table: {
                Parser::step_create_table();
                break;
            }
            case Step::create_table_opening_parens: {
                Parser::step_create_table_opening_parens();
                break;
            }
            case Step::create_table_field_name: {
                Parser::step_create_table_field_name();
                break;
            }
            case Step::create_table_field_type: {
                Parser::step_create_table_field_type();
                break;
            }
            case Step::create_table_comma_or_closing_parens: {
                Parser::step_create_table_comma_or_closing_parens();
                break;
            }
            case Step::update_table: {
                Parser::step_update_table();
                break;
            }
            case Step::update_set: {
                Parser::step_update_set();
                break;
            }
            case Step::update_field: {
                Parser::step_update_field();
                break;
            }
            case Step::update_equals: {
                Parser::step_update_equals();
                break;
            }
            case Step::update_value: {
                Parser::step_update_value();
                break;
            }
            case Step::update_comma: {
                Parser::step_update_comma();
                break;
            }
            case Step::where: {
                Parser::step_where();
                break;
            }
            case Step::where_field: {
                Parser::step_where_field();
                break;
            }
            case Step::where_operator: {
                Parser::step_where_operator();
                break;
            }
            case Step::where_value: {
                Parser::step_where_value();
                break;
            }
            case Step::where_and: {
                Parser::step_where_and();
                break;
            }
            case Step::group_by: {
                Parser::step_group_by();
                break;
            }
            case Step::group_by_field: {
                Parser::step_group_by_field();
                break;
            }
            case Step::having: {
                Parser::step_having();
                break;
            }
            case Step::having_field: {
                Parser::step_having_field();
                break;
            }
            case Step::having_operator: {
                Parser::step_having_operator();
                break;
            }
            case Step::having_value: {
                Parser::step_having_value();
                break;
            }
            case Step::join: {
                Parser::step_join();
                break;
            }
            case Step::join_table: {
                Parser::step_join_table();
                break;
            }
            case Step::join_on: {
                Parser::step_join_on();
                break;
            }
            case Step::join_on_field1: {
                Parser::step_join_on_field1();
                break;
            }
            case Step::join_on_equal: {
                Parser::step_join_on_equal();
                break;
            }
            case Step::join_on_field2: {
                Parser::step_join_on_field2();
                break;
            }
            case Step::insert_fields_opening_parens: {
                Parser::step_insert_fields_opening_parens();
                break;
            }
            case Step::insert_fields: {
                Parser::step_insert_fields();
                break;
            }
            case Step::insert_fields_comma_or_closing_parens: {
                Parser::step_insert_fields_comma_or_closing_parens();
                break;
            }
            case Step::insert_values_rword: {
                Parser::step_insert_values_rword();
                break;
            }
            case Step::insert_values_opening_parens: {
                Parser::step_insert_values_opening_parens();
                break;
            }
            case Step::insert_values: {
                Parser::step_insert_values();
                break;
            }
            case Step::insert_values_comma_or_closing_parens: {
                Parser::step_insert_values_comma_or_closing_parens();
                break;
            }
            case Step::insert_values_comma_before_opening_parens: {
                Parser::step_insert_values_comma_before_opening_parens();
                break;
            }
            case Step::primary_key_as_type: {
                Parser::step_primary_key_as_type();
                break;
            }
            case Step::primary_key_at_end: {
                Parser::step_primary_key_at_end();
                break;
            }
            case Step::primary_key_opening_parens: {
                Parser::step_primary_key_opening_parens();
                break;
            }
            case Step::primary_key_field: {
                Parser::step_primary_key_field();
                break;
            }
            case Step::primary_key_comma_or_closing_parens: {
                Parser::step_primary_key_comma_or_closing_parens();
                break;
            }
            case Step::foreign_key: {
                Parser::step_foreign_key();
                break;
            }
            case Step::foreign_key_as_type: {
                Parser::step_foreign_key_as_type();
                break;
            }
            case Step::references: {
                Parser::step_references();
                break;
            }
            case Step::references_table: {
                Parser::step_references_table();
                break;
            }
            case Step::references_table_opening_parens: {
                Parser::step_references_table_opening_parens();
                break;
            }
            case Step::references_field: {
                Parser::step_references_field();
                break;
            }
            case Step::references_comma_or_closing_parens: {
                Parser::step_references_comma_or_closing_parens();
                break;
            }
            case Step::foreign_key_opening_parens: {
                Parser::step_foreign_key_opening_parens();
                break;
            }
            case Step::foreign_key_field: {
                Parser::step_foreign_key_field();
                break;
            }
            case Step::foreign_key_comma_or_closing_parens: {
                Parser::step_foreign_key_comma_or_closing_parens();
                break;
            }
            case Step::error: {
                return Parser::step_error();
            }
        }
    }
};