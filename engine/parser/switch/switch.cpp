//
// Created by Altezza on 17.05.2024.
//
#include "../parser.hpp"

std::pair<query, parse_error> parser::parser_switch() {
    while (true) {
        if (is_index_at_end()) return std::make_pair(parser::q, parser::error);
        switch (parser::step) {
            case step::type: {
                parser::step_type();
                break;
            }
            case step::select_field: {
                parser::step_select_field();
                break;
            }
            case step::select_comma: {
                parser::step_select_comma();
                break;
            }
            case step::select_from : {
                parser::step_select_from();
                break;
            }
            case step::select_from_table: {
                parser::step_select_from_table();
                break;
            }
            case step::insert_table: {
                parser::step_insert_table();
                break;
            }
            case step::delete_from_table: {
                parser::step_delete_from_table();
                break;
            }
            case step::create_table: {
                parser::step_create_table();
                break;
            }
            case step::create_table_opening_parens: {
                parser::step_create_table_opening_parens();
                break;
            }
            case step::create_table_field_name: {
                parser::step_create_table_field_name();
                break;
            }
            case step::create_table_field_type: {
                parser::step_create_table_field_type();
                break;
            }
            case step::create_table_comma_or_closing_parens: {
                parser::step_create_table_comma_or_closing_parens();
                break;
            }
            case step::update_table: {
                parser::step_update_table();
                break;
            }
            case step::update_set: {
                parser::step_update_set();
                break;
            }
            case step::update_field: {
                parser::step_update_field();
                break;
            }
            case step::update_equals: {
                parser::step_update_equals();
                break;
            }
            case step::update_value: {
                parser::step_update_value();
                break;
            }
            case step::update_comma: {
                parser::step_update_comma();
                break;
            }
            case step::where: {
                parser::step_where();
                break;
            }
            case step::where_field: {
                parser::step_where_field();
                break;
            }
            case step::where_operator: {
                parser::step_where_operator();
                break;
            }
            case step::where_value: {
                parser::step_where_value();
                break;
            }
            case step::where_and: {
                parser::step_where_and();
                break;
            }
            case step::group_by: {
                parser::step_group_by();
                break;
            }
            case step::group_by_field: {
                parser::step_group_by_field();
                break;
            }
            case step::having: {
                parser::step_having();
                break;
            }
            case step::having_field: {
                parser::step_having_field();
                break;
            }
            case step::having_operator: {
                parser::step_having_operator();
                break;
            }
            case step::having_value: {
                parser::step_having_value();
                break;
            }
            case step::join: {
                parser::step_join();
                break;
            }
            case step::join_table: {
                parser::step_join_table();
                break;
            }
            case step::join_on: {
                parser::step_join_on();
                break;
            }
            case step::join_on_field1: {
                parser::step_join_on_field1();
                break;
            }
            case step::join_on_equal: {
                parser::step_join_on_equal();
                break;
            }
            case step::join_on_field2: {
                parser::step_join_on_field2();
                break;
            }
            case step::insert_fields_opening_parens: {
                parser::step_insert_fields_opening_parens();
                break;
            }
            case step::insert_fields: {
                parser::step_insert_fields();
                break;
            }
            case step::insert_fields_comma_or_closing_parens: {
                parser::step_insert_fields_comma_or_closing_parens();
                break;
            }
            case step::insert_values_rword: {
                parser::step_insert_values_rword();
                break;
            }
            case step::insert_values_opening_parens: {
                parser::step_insert_values_opening_parens();
                break;
            }
            case step::insert_values: {
                parser::step_insert_values();
                break;
            }
            case step::insert_values_comma_or_closing_parens: {
                parser::step_insert_values_comma_or_closing_parens();
                break;
            }
            case step::insert_values_comma_before_opening_parens: {
                parser::step_insert_values_comma_before_opening_parens();
                break;
            }
            case step::primary_key_as_type: {
                parser::step_primary_key_as_type();
                break;
            }
            case step::primary_key_at_end: {
                parser::step_primary_key_at_end();
                break;
            }
            case step::primary_key_opening_parens: {
                parser::step_primary_key_opening_parens();
                break;
            }
            case step::primary_key_field: {
                parser::step_primary_key_field();
                break;
            }
            case step::primary_key_comma_or_closing_parens: {
                parser::step_primary_key_comma_or_closing_parens();
                break;
            }
            case step::foreign_key: {
                parser::step_foreign_key();
                break;
            }
            case step::foreign_key_as_type: {
                parser::step_foreign_key_as_type();
                break;
            }
            case step::references: {
                parser::step_references();
                break;
            }
            case step::references_table: {
                parser::step_references_table();
                break;
            }
            case step::references_table_opening_parens: {
                parser::step_references_table_opening_parens();
                break;
            }
            case step::references_field: {
                parser::step_references_field();
                break;
            }
            case step::references_comma_or_closing_parens: {
                parser::step_references_comma_or_closing_parens();
                break;
            }
            case step::foreign_key_opening_parens: {
                parser::step_foreign_key_opening_parens();
                break;
            }
            case step::foreign_key_field: {
                parser::step_foreign_key_field();
                break;
            }
            case step::foreign_key_comma_or_closing_parens: {
                parser::step_foreign_key_comma_or_closing_parens();
                break;
            }
            case step::error: {
                return parser::step_error();
            }
        }
    }
};