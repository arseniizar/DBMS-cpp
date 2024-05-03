//
// Created by Altezza on 29.04.2024.
//

#include "parser.h"
#include <bits/stdc++.h>
#include <fmt/core.h>

const std::deque<std::string> &parser::get_tokens() const {
    return tokens;
}

void parser::set_tokens(const std::deque<std::string> &tokens) {
    parser::tokens = tokens;
}

const std::string &parser::get_input() const {
    return sql;
}

void parser::set_input(const std::string &input) {
    parser::sql = input;
}

const query &parser::get_query() const {
    return query;
}

void parser::set_query(const struct query &query) {
    parser::query = query;
}

size_t parser::get_index() const {
    return index;
}

void parser::set_index(size_t index) {
    parser::index = index;
}

step parser::get_step() const {
    return step;
}

void parser::set_step(enum step step) {
    parser::step = step;
}

// auxiliary functions

auto is_identifier(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    for (auto const &word: reserved_words) {
        if (str == word) return false;
    }
    auto match = std::smatch();
    auto regex = std::regex("[a-zA-Z_][a-zA-Z_0-9]*");
    auto matched = std::regex_match(str, match, regex);
    return matched;
}

auto is_identifier_or_asterisk(const std::string &str) {
    return is_identifier(str) || str == "*";
};

auto str_toupper(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

auto parser::make_error_pair(std::string const &message) {
    return std::make_pair(parser::query, parse_error(message));
}

// so I will try to make my own LL(1) parser!

// test lexeme (lexical analysis)
void parser::lexeme() {
    using namespace std;
    if (parser::sql.empty()) {
        fmt::println("Input should not be empty!");
    }
    auto parse_tokens = stringstream(parser::sql);
    auto containsOperator = [](string &str) {
        for (char &c: str)
            if (c >= '!' and c <= '/') return c;
        return '\0';
    };
    for (auto token = string(); parse_tokens >> token;) {
        auto token_operator = containsOperator(token);
        if (token_operator != '\0') {
            token.erase(token.find(token_operator));
            parser::tokens.push_back(token);
            parser::tokens.emplace_back(1, token_operator);
        } else {
            parser::tokens.push_back(token);
        }
    }
}

std::string parser::pop() {
    auto pair = parser::peek_with_length();
    parser::index += pair.second;
    parser::pop_whitespace();
    return pair.first;
}

void parser::pop_whitespace() {
    for (;
            parser::index < parser::sql.size() && parser::sql.at(parser::index) == ' ';
            parser::index++);
}

std::string parser::peek() {
    auto peeked = parser::peek_with_length();
    return peeked.first;
}

std::pair<std::string, int> parser::peek_with_length() {
    if (parser::index >= parser::sql.size()) return std::make_pair("", 0);
    for (auto const &word: reserved_words) {
        auto token = parser::sql.substr
                (std::min(parser::sql.size(), parser::sql.size() + word.size()));
        str_toupper(token);
        if (token == word) {
            parser::tokens.push_back(token);
            return std::make_pair(token, token.size());
        }
    }
    // quoted string
    if (parser::sql[parser::index] == '\'') {
        return parser::peek_quoted_with_length();
    }
    // identifier
    return parser::peek_identifier_with_length();
}

std::pair<std::string, int> parser::peek_quoted_with_length() {
    if (parser::sql.size() < parser::index || parser::sql.at(index) != '\'')
        return std::make_pair("", 0);
    for (auto i = parser::index + 1; i < parser::sql.length(); i++) {
        if (parser::sql.at(i) == '\'' && parser::sql.at(i - 1) == '\\') {
            auto length = parser::sql.substr(parser::index, i).size() + 2;
            return std::make_pair(parser::sql.substr(parser::index, i), length);
        }
    }
    return std::make_pair("", 0);
}

std::pair<std::string, int> parser::peek_identifier_with_length() {
    for (auto i = parser::index; i < parser::sql.size(); ++i) {
        auto regex = std::regex("[a-zA-Z0-9_*]");
        auto match = std::smatch();
        auto str = std::string(1, parser::sql.at(parser::index));
        auto matched = std::regex_match(str, match, regex);
        if (!matched) {
            auto substr = parser::sql.substr(parser::index, i);
            return std::make_pair(substr, substr.size());
        }
    }
    auto substr = parser::sql.substr(parser::index);
    return std::make_pair(substr, substr.size());
}

struct query parser::parse() {
    auto pair = parser::do_parse();
    parser::error = pair.second;

}

std::pair<struct query, struct parse_error> parser::do_parse() {
    while (true) {
        if (parser::index >= parser::sql.size())
            return std::make_pair(parser::query, parser::error);
        switch (parser::step) {
            case step::type: {
                auto peeked = parser::peek();
                str_toupper(peeked);
                if (peeked == "SELECT") {
                    parser::query.set_type(query_type::Select);
                    parser::pop();
                    parser::step = step::select_field;
                } else if (peeked == "INSERT INTO") {
                    parser::query.set_type(query_type::Insert);
                    parser::pop();
                    parser::step = step::insert_table;
                } else if (peeked == "UPDATE") {
                    parser::query.set_type(query_type::Update);
                    parser::pop();
                    parser::step = step::update_table;
                } else if (peeked == "DELETE FROM") {
                    parser::query.set_type(query_type::Delete);
                    parser::pop();
                    parser::step = step::delete_from_table;
                } else {
                    return parser::make_error_pair("Invalid query type");
                }
            }
            case step::select_field: {
                auto identifier = parser::peek();
                if (!is_identifier(identifier))
                    return parser::make_error_pair("at SELECT: expected field to SELECT");
                parser::query.append_field(identifier);
                parser::pop();
                auto from = parser::peek();
                str_toupper(from);
                if (from == "AS") {
                    parser::pop();
                    auto alias = parser::peek();
                    if (!is_identifier(alias))
                        return parser::make_error_pair("at SELECT: expected field alias for "
                                                       + identifier + " as to SELECT");
                    parser::query.append_alias(identifier, alias);
                    parser::pop();
                    from = parser::peek();
                }
                str_toupper(from);
                if (from == "FROM") {
                    parser::step = step::select_from;
                    continue;
                }
                parser::step = step::select_comma;
            }
            case step::select_comma: {
                auto comma = parser::peek();
                if (comma != ",") {
                    return parser::make_error_pair("at SELECT: expected comma or FROM");
                }
                parser::pop();
                parser::step = step::select_field;
            }
            case step::select_from : {
                auto from = parser::peek();
                str_toupper(from);
                if(from != "FROM") {
                    return make_error_pair("at SELECT: expected FROM");
                }
                parser::pop();
                parser::step = step::select_from_table;
            }
            case step::select_from_table : {
                auto table_name = parser::peek();
                if(table_name.empty()) {
                    return make_error_pair("at SELECT: expected quoted table name");
                }
                parser::query.set_table_name(table_name);
                parser::pop();
                parser::step = step::where;
            }
            case step::insert_table: {
                auto table_name = parser::peek();
                if(table_name.empty()) {
                    return make_error_pair("at INSERT INTO: expected quoted table name");
                }
                parser::query.set_table_name(table_name);
                parser::pop();
                parser::step = step::insert_fields_opening_parens;
            }
            case step::delete_from_table: {
                auto table_name = parser::peek();
                if(table_name.empty()) {
                    return make_error_pair("at DELETE FROM: expected quoted table name");
                }
                parser::query.set_table_name(table_name);
                parser::pop();
                parser::step = step::where;
            }
            case step::update_table: {
                 auto table_name = parser::peek();
                if(table_name.empty()) {
                    return make_error_pair("at UPDATE: expected quoted table name");
                }
                parser::query.set_table_name(table_name);
                parser::pop();
                parser::step = step::update_set;
            }
            case step::update_set: {
                auto set = parser::peek();
                if(set != "SET") {
                    return make_error_pair("at UPDATE: expected 'SET'");
                }
                parser::pop();
                parser::step = step::update_field;
            }
            case step::update_field: {
                auto identifier = parser::peek();
                if(!is_identifier(identifier))
                    return make_error_pair("at UPDATE: expected at least one field to update");
                parser::next_update_field = parser::peek();
                parser::pop();
                parser::step = step::update_equals;
            }
            case step::update_equals: {
                auto equals = parser::peek();
                if(equals != "=")
                    return make_error_pair("at UPDATE: expected '='");
                parser::pop();
                parser::step = step::update_value;
            }
            case step::update_value: {

            }
        }
    }
}

