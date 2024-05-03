//
// Created by Altezza on 29.04.2024.
//

#ifndef DATABASEPROJECT_PARSER_H
#define DATABASEPROJECT_PARSER_H


#include <string>
#include <utility>
#include <vector>
#include <deque>
#include "../query.h"
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
    where_and
};

inline std::string reserved_words[]{
        "(", ")", ">=", "<=", "!=",
        ",", "=", ">", "<", "SELECT",
        "INSERT INTO", "VALUES", "UPDATE",
        "DELETE FROM", "WHERE", "FROM",
        "SET", "AS",
};

inline std::string start_words[]{
        "SELECT", "INSERT INTO", "UPDATE", "DELETE FROM"
};

struct parser {
private:
    std::deque<std::string> tokens;
    std::string sql;
    std::string next_update_field;
    query query;
    size_t index;
    step step;
    parse_error error;

public:
    explicit parser()
            : sql("none"), index(0), step(step::type) {}


    void input(std::string const &sql) {
        parser::sql = sql;
    }

    void lexeme();

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