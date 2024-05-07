//
// Created by Altezza on 29.04.2024.
//

#ifndef DATABASEPROJECT_PARSER_H
#define DATABASEPROJECT_PARSER_H


#include <string>
#include <utility>
#include <vector>
#include <deque>
#include "../query/query.h"
#include "parse_error.h"

enum struct step {
    type,
    select_field,
    select_from,
    select_comma,
    select_from_table,
    insert_table,
    insert_fields_opening_parens,
    insert_fields,
    insert_field_type,
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
};

inline std::string reserved_words[]{
        "(", ")", ">=", "<=", "!=",
        ",", "=", ">", "<", "SELECT",
        "INSERT INTO", "VALUES", "UPDATE",
        "DELETE FROM", "CREATE TABLE", "WHERE",
        "FROM", "SET", "AS", "NVARCHAR2",
        "DATE", "INTEGER"
};

inline std::string start_words[]{
        "SELECT", "INSERT INTO", "UPDATE", "DELETE FROM"
};

struct parser {
private:
    std::deque<std::string> tokens;
    std::string sql;
    std::string next_update_field;
    std::string current_create_table_field;
    query q;
    size_t index;
    step step;
    parse_error error;

public:
    explicit parser()
            : sql(std::string()), index(0), step(step::type) {}

    void clean() {
        parser::tokens.erase(parser::tokens.begin(), parser::tokens.end());
        parser::sql = std::string();
        parser::next_update_field = std::string();
        parser::current_create_table_field = std::string();
        parser::q = query();
        parser::index = std::size_t(0);
        parser::step = step::type;
        parser::error = parse_error();
    }

    void input(std::string const &sql) {
        parser::sql = sql;
    }

    auto make_error_pair(std::string const &message);

    std::string peek();

    std::pair<std::string, int> peek_with_length();

    std::pair<std::string, int> peek_quoted_with_length();

    std::pair<std::string, int> peek_identifier_with_length();

    std::string pop();

    void pop_whitespace();

    struct query parse();

    std::pair<struct query, struct parse_error> do_parse();

    [[nodiscard]] const parse_error &get_error() const;
};


#endif //DATABASEPROJECT_PARSER_H