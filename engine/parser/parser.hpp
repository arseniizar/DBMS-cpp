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
#include "parse_error.hpp"

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
    error,
};

inline std::vector<std::string> reserved_words{
        "(", ")", ">=", "<=", "!=",
        ",", "=", ">", "<", "SELECT",
        "INSERT INTO", "VALUES", "UPDATE",
        "DELETE FROM", "CREATE TABLE", "WHERE",
        "FROM", "SET", "AS", "NVARCHAR2",
        "DATE", "INTEGER", "GROUP BY", "HAVING",
        "JOIN", "LEFT JOIN", "RIGHT JOIN", "FULL JOIN",
        "INNER JOIN", "LEFT OUTER JOIN", "RIGHT OUTER JOIN",
        "FULL OUTER JOIN", "ON"
};

struct parser {
private:
    std::string error_message;
    std::string sql;
    std::string next_update_field;
    std::string current_create_table_field;
    std::string current_peeked;
    query q;
    size_t index;
    step step;
    parse_error error;
    bool pop_flag;

public:
    explicit parser()
            : sql(std::string()), index(0), step(step::type), pop_flag(false) {}

    auto make_error_pair(std::string const &message);

    void clean();

    void clean_error();

    void input(std::string const &sql);

    std::string peek();

    std::pair<std::string, int> peek_with_length();

    std::pair<std::string, int> peek_quoted_with_length();

    std::pair<std::string, int> peek_identifier_with_length();

    std::string pop();

    void pop_whitespace();

    struct query parse();

    std::pair<struct query, struct parse_error> do_parse();

    [[nodiscard]] parse_error get_error();

    auto split_string_in_words();

    // complementary functions:
    bool peek_is_comma(std::string const &message);

    bool is_index_at_end();

    bool is_peek_empty();

    auto parser_switch();

    //switch case functions
    auto step_type();

    auto step_select_field();

    auto step_select_from();

    auto step_select_comma();

    auto step_select_from_table();

    auto step_insert_table();

    auto step_insert_fields_opening_parens();

    auto step_insert_fields();

    auto step_insert_fields_comma_or_closing_parens();

    auto step_insert_values_opening_parens();

    auto step_insert_values_rword();

    auto step_insert_values();

    auto step_insert_values_comma_or_closing_parens();

    auto step_insert_values_comma_before_opening_parens();

    auto step_update_table();

    auto step_update_set();

    auto step_update_field();

    auto step_update_equals();

    auto step_update_value();

    auto step_update_comma();

    auto step_delete_from_table();

    auto step_where();

    auto step_where_field();

    auto step_where_operator();

    auto step_where_value();

    auto step_where_and();

    auto step_create_table();

    auto step_create_table_opening_parens();

    auto step_create_table_field_name();

    auto step_create_table_field_type();

    auto step_create_table_comma_or_closing_parens();

    auto step_group_by();
    auto step_group_by_field();
    auto step_having();
    auto step_having_field();
    auto step_having_operator();
    auto step_having_value();
    auto step_join();
    auto step_join_table();
    auto step_join_on();
    auto step_join_on_field1();
    auto step_join_on_equal();
    auto step_join_on_field2();

    auto step_error();
};


#endif //DATABASEPROJECT_PARSER_HPP