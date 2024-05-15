//
// Created by Altezza on 14.05.2024.
//

#ifndef DATABASEPROJECT_PARSER_SWITCH_HPP
#define DATABASEPROJECT_PARSER_SWITCH_HPP

#include "parser.hpp"
#include "parser_complementary.hpp"
#include "parser_steps.hpp"

auto parser::parser_switch() {
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
            case step::error: {
                return parser::step_error();
            }
        }
    }
};


#endif //DATABASEPROJECT_PARSER_SWITCH_HPP
