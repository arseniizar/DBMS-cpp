//
// Created by Altezza on 29.04.2024.
//

#ifndef DATABASEPROJECT_PARSER_HPP
#define DATABASEPROJECT_PARSER_HPP


#include <string>
#include <utility>
#include <vector>
#include <deque>
#include "../query/query.hpp"
#include "structs/parse_error.hpp"

enum struct step {
    type,
    select_field,
    select_from,
    select_comma,
    select_from_table,
    insert_table,
    insert_fields_opening_parens,
    insert_fields,
    insert_fields_comma_or_closing_parens,
    insert_values_opening_parens,
    insert_values_rword,
    insert_values,
    insert_values_comma_or_closing_parens,
    insert_values_comma_before_opening_parens,
    update_table,
    update_set,
    update_field,
    update_equals,
    update_value,
    update_comma,
    delete_from_table,
    where,
    where_field,
    where_operator,
    where_value,
    where_and,
    create_table,
    create_table_opening_parens,
    create_table_field_name,
    create_table_field_type,
    create_table_comma_or_closing_parens,
    group_by,
    group_by_field,
    having,
    having_field,
    having_operator,
    having_value,
    join,
    join_table,
    join_on,
    join_on_field1,
    join_on_equal,
    join_on_field2,
    primary_key_as_type,
    primary_key_at_end,
    primary_key_opening_parens,
    primary_key_field,
    primary_key_comma_or_closing_parens,
    foreign_key,
    foreign_key_as_type,
    foreign_key_opening_parens,
    foreign_key_field,
    foreign_key_comma_or_closing_parens,
    references,
    references_table,
    references_table_opening_parens,
    references_field,
    references_comma_or_closing_parens,
    error,
};

inline std::vector<std::string> reserved_words{
        "(", ")", ">=", "<=", "!=",
        ",", "=", ">", "<", "SELECT",
        "INSERT INTO", "VALUES", "UPDATE",
        "DELETE FROM", "CREATE TABLE", "WHERE",
        "FROM", "SET", "AS", "NVARCHAR2",
        "DATE", "INTEGER", "GROUP BY", "HAVING",
        "JOIN", "LEFT JOIN", "RIGHT JOIN", "FULL JOIN", "ON",
        "PRIMARY KEY", "FOREIGN KEY", "REFERENCES"
};

struct parser {
private:
    std::string error_message;
    std::string sql;
    std::string next_update_field;
    std::string current_create_table_field_val;
    std::string current_references_table_name;
    std::string current_peeked;
    data_type current_field_data_type;
    query q;
    size_t index;
    step step;
    parse_error error;
    bool pop_flag;

public:
    explicit parser()
            : sql(std::string()), index(0), step(step::type), pop_flag(false),
              current_field_data_type(data_type::UNKNOWN) {}

    std::pair<query, parse_error> make_error_pair(std::string const &message);

    void clean();

    void clean_error();

    void input(std::string const &sql);

    std::string peek();

    std::pair<std::string, int> peek_with_length();

    std::pair<std::string, int> peek_quoted_with_length();

    std::pair<std::string, int> peek_identifier_with_length();

    std::string pop();

    void pop_whitespace();

    query parse();

    std::pair<struct query, struct parse_error> do_parse();

    [[nodiscard]] parse_error get_error();

    std::vector<std::string> split_string_in_words();

    // complementary functions:
    bool peek_is_comma(std::string const &message);

    __gnu_cxx::__normal_iterator<field *, std::vector<field>>
    get_field_by_name(std::string const &name);

    bool peek_is_table_name(std::string const &message);

    bool peek_is_opening_parens(std::string const &message);

    bool is_index_at_end();

    bool is_peek_empty();

    std::pair<query, parse_error> parser_switch();

    bool peek_is_operator(std::string const &message);

    void set_additional_step();

    //switch case functions
    void step_type();

    void step_select_field();

    void step_select_from();

    void step_select_comma();

    void step_select_from_table();

    void step_insert_table();

    void step_insert_fields_opening_parens();

    void step_insert_fields();

    void step_insert_fields_comma_or_closing_parens();

    void step_insert_values_opening_parens();

    void step_insert_values_rword();

    void step_insert_values();

    void step_insert_values_comma_or_closing_parens();

    void step_insert_values_comma_before_opening_parens();

    void step_update_table();

    void step_update_set();

    void step_update_field();

    void step_update_equals();

    void step_update_value();

    void step_update_comma();

    void step_delete_from_table();

    void step_where();

    void step_where_field();

    void step_where_operator();

    void step_where_value();

    void step_where_and();

    void step_create_table();

    void step_create_table_opening_parens();

    void step_create_table_field_name();

    void step_create_table_field_type();

    void step_create_table_comma_or_closing_parens();

    void step_group_by();

    void step_group_by_field();

    void step_having();

    void step_having_field();

    void step_having_operator();

    void step_having_value();

    void step_join();

    void step_join_table();

    void step_join_on();

    void step_join_on_field1();

    void step_join_on_equal();

    void step_join_on_field2();

    void step_primary_key_as_type();

    void step_primary_key_at_end();

    void step_primary_key_opening_parens();

    void step_primary_key_field();

    void step_primary_key_comma_or_closing_parens();

    void step_foreign_key();

    void step_foreign_key_as_type();

    void step_foreign_key_opening_parens();

    void step_foreign_key_field();

    void step_foreign_key_comma_or_closing_parens();

    void step_references();

    void step_references_table();

    void step_references_table_opening_parens();

    void step_references_field();

    void step_references_comma_or_closing_parens();

    std::pair<query, parse_error> step_error();
};


#endif //DATABASEPROJECT_PARSER_HPP