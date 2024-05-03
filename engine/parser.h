//
// Created by Altezza on 29.04.2024.
//

#ifndef DATABASEPROJECT_PARSER_H
#define DATABASEPROJECT_PARSER_H


#include <string>
#include <utility>
#include <vector>
#include <deque>
#include "query.h"
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

inline std::string reserved_words[] {
	"(", ")", ">=", "<=", "!=",
    ",", "=", ">", "<", "SELECT",
    "INSERT INTO","VALUES", "UPDATE",
    "DELETE FROM","WHERE", "FROM",
    "SET", "AS",
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
    explicit parser(std::string input)
            : sql(std::move(input)), index(0), step(step::type) {}

    void lexeme();

    auto make_error_pair(std::string const& message);

    std::string peek();

    std::pair<std::string, int> peek_with_length();
    std::pair<std::string, int> peek_quoted_with_length();
    std::pair<std::string, int> peek_identifier_with_length();

    std::string pop();

    void pop_whitespace();

    struct query parse();
    std::pair<struct query, struct parse_error> do_parse();

    const std::deque<std::string> &get_tokens() const;

    void set_tokens(const std::deque<std::string> &tokens);

    const std::string &get_input() const;

    void set_input(const std::string &input);

    const struct query &get_query() const;

    void set_query(const struct query &query);

    size_t get_index() const;

    void set_index(size_t index);

    enum step get_step() const;

    void set_step(enum step step);
};


#endif //DATABASEPROJECT_PARSER_H