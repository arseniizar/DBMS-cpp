//
// Created by Altezza on 29.04.2024.
//

#include "parser.h"
#include <bits/stdc++.h>
#include <fmt/core.h>

// parser struct generated functions

const parse_error &parser::get_error() const {
    return error;
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
        auto end_index = std::min(parser::sql.size(), parser::index + word.size());
        auto token = parser::sql.substr(parser::index, end_index - parser::index);
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
    for (auto i = parser::index + 1; i < parser::sql.size(); i++) {
        if (parser::sql.at(i) == '\'') {
            auto substr = parser::sql.substr(parser::index + 1, i - parser::index - 1);
            return std::make_pair(substr, substr.size() + 2);
        }
    }
    return std::make_pair("", 0);
}

std::pair<std::string, int> parser::peek_identifier_with_length() {
    for (auto i = parser::index; i < parser::sql.size(); ++i) {
        auto regex = std::regex("[a-zA-Z0-9_*]");
        auto match = std::smatch();
        auto str = std::string(1, parser::sql.at(i));
        auto matched = std::regex_match(str, match, regex);
        if (!matched) {
            auto substr = parser::sql.substr(parser::index, i - parser::index);
            return std::make_pair(substr, substr.size());
        }
    }
    auto substr = parser::sql.substr(parser::index);
    return std::make_pair(substr, substr.size());
}

struct query parser::parse() {
    auto pair = parser::do_parse();
    parser::error = pair.second;
    return pair.first;
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
                if (!is_identifier_or_asterisk(identifier))
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
                break;
            }
            case step::select_from : {
                auto from = parser::peek();
                str_toupper(from);
                if (from != "FROM") {
                    return make_error_pair("at SELECT: expected FROM");
                }
                parser::pop();
                parser::step = step::select_from_table;
                break;
            }
            case step::select_from_table : {
                auto table_name = parser::peek();
                if (table_name.empty()) {
                    return make_error_pair("at SELECT: expected quoted table name");
                }
                parser::query.set_table_name(table_name);
                parser::pop();
                parser::step = step::where;
                break;
            }
            case step::insert_table: {
                auto table_name = parser::peek();
                if (table_name.empty()) {
                    return make_error_pair("at INSERT INTO: expected quoted table name");
                }
                parser::query.set_table_name(table_name);
                parser::pop();
                parser::step = step::insert_fields_opening_parens;
                break;
            }
            case step::delete_from_table: {
                auto table_name = parser::peek();
                if (table_name.empty()) {
                    return make_error_pair("at DELETE FROM: expected quoted table name");
                }
                parser::query.set_table_name(table_name);
                parser::pop();
                parser::step = step::where;
                break;
            }
            case step::update_table: {
                auto table_name = parser::peek();
                if (table_name.empty()) {
                    return make_error_pair("at UPDATE: expected quoted table name");
                }
                parser::query.set_table_name(table_name);
                parser::pop();
                parser::step = step::update_set;
                break;
            }
            case step::update_set: {
                auto set = parser::peek();
                if (set != "SET") {
                    return make_error_pair("at UPDATE: expected 'SET'");
                }
                parser::pop();
                parser::step = step::update_field;
                break;
            }
            case step::update_field: {
                auto identifier = parser::peek();
                if (!is_identifier(identifier))
                    return make_error_pair("at UPDATE: expected at least one field to update");
                parser::next_update_field = parser::peek();
                parser::pop();
                parser::step = step::update_equals;
                break;
            }
            case step::update_equals: {
                auto equals = parser::peek();
                if (equals != "=")
                    return make_error_pair("at UPDATE: expected '='");
                parser::pop();
                parser::step = step::update_value;
                break;
            }
            case step::update_value: {
                auto pair = parser::peek_quoted_with_length();
                if (pair.second == 0)
                    return make_error_pair("at UPDATE: expected quoted value");
                parser::query.append_update(parser::next_update_field, pair.first);
                parser::next_update_field.erase();
                parser::pop();
                auto where = parser::peek();
                str_toupper(where);
                if (where != "WHERE") {
                    parser::step = step::where;
                    continue;
                }
                parser::step = step::update_comma;
                break;
            }
            case step::update_comma: {
                auto comma = parser::peek();
                if (comma != ",")
                    return make_error_pair("at UPDATE: expected ','");
                parser::pop();
                parser::step = step::update_field;
                break;
            }
            case step::where: {
                auto where = parser::peek();
                str_toupper(where);
                if (where != "WHERE")
                    return make_error_pair("expected WHERE");
                parser::pop();
                parser::step = step::where_field;
                break;
            }
            case step::where_field: {
                auto identifier = parser::peek();
                if (!is_identifier(identifier))
                    return make_error_pair("at WHERE: expected field");
                parser::query.append_condition(condition(identifier, true));
                parser::pop();
                parser::step = step::where_operator;
                break;
            }
            case step::where_operator: {
                auto op = parser::peek();
                auto curr_cond = parser::query.get_current_condition();
                if (op == "=") curr_cond.query_operator = query_operator::eq;
                else if (op == ">") curr_cond.query_operator = query_operator::gt;
                else if (op == ">=") curr_cond.query_operator = query_operator::gte;
                else if (op == "<") curr_cond.query_operator = query_operator::lt;
                else if (op == "<=") curr_cond.query_operator = query_operator::lte;
                else if (op == "!=") curr_cond.query_operator = query_operator::ne;
                else return make_error_pair("at WHERE: unknown operator");
                parser::query.set_current_condition(curr_cond);
                parser::pop();
                parser::step = step::where_value;
                break;
            }
            case step::where_value: {
                auto curr_cond = parser::query.get_current_condition();
                auto identifier = parser::peek();
                if(is_identifier(identifier)) {
                    curr_cond.operand2 = identifier;
                    curr_cond.operand2_is_field = true;
                } else {
                    auto pair = parser::peek_quoted_with_length();
                    if(pair.second == 0)
                        return make_error_pair("at WHERE: expected quoted value");
                    curr_cond.operand2 = pair.first;
                    curr_cond.operand2_is_field = false;
                }
                parser::query.set_current_condition(curr_cond);
                parser::pop();
                parser::step = step::where_and;
                break;
            }
            case step::where_and: {
                auto and_word = parser::peek();
                str_toupper(and_word);
                if(and_word != "AND")
                    return make_error_pair("expected AND");
                parser::pop();
                parser::step = step::where_field;
                break;
            }
            case step::insert_fields_opening_parens: {
                auto opening_parens = parser::peek();
                if(opening_parens != "(" || opening_parens.size() != 1)
                    return make_error_pair("at INSERT INTO: expected opening parens");
                parser::pop();
                parser::step = step::insert_fields;
                break;
            }
            case step::insert_fields: {
                auto identifier = parser::peek();
                if(!is_identifier(identifier))
                    return make_error_pair("at INSERT INTO: expected at least one field to insert");
                parser::query.append_field(identifier);
                parser::pop();
                parser::step = step::insert_fields_comma_or_closing_parens;
                break;
            }
            case step::insert_fields_comma_or_closing_parens: {
                auto comma_or_closing_paren = parser::peek();
                if(comma_or_closing_paren != "," or comma_or_closing_paren != ")")
                    return make_error_pair("at INSERT INTO: expected comma or closing parens");
                parser::pop();
                if(comma_or_closing_paren == ",") {
                    parser::step = step::insert_fields;
                    continue;
                }
                parser::step = step::insert_values_rword;
                break;
            }
            case step::insert_values_rword: {
                auto values = parser::peek();
                str_toupper(values);
                if(values != "VALUES")
                    return make_error_pair("at INSERT INTO: expected 'VALUES'");
                parser::pop();
                parser::step = step::insert_values_opening_parens;
                break;
            }
            case step::insert_values_opening_parens: {
                auto opening_parens = parser::peek();
                if(opening_parens != "(")
                    return make_error_pair("at INSERT INTO: expected opening parens");
                parser::query.append_inserts_vec(std::vector<std::string>());
                parser::pop();
                parser::step = step::insert_values;
                break;
            }
            case step::insert_values: {
                auto pair = parser::peek_quoted_with_length();
                if(pair.second == 0 )
                    return make_error_pair("at INSERT INTO: expected quoted value");
                parser::query.append_insert(pair.first);
                parser::pop();
                parser::step = step::insert_values_comma_or_closing_parens;
                break;
            }
            case step::insert_values_comma_or_closing_parens: {
                auto comma_or_closing_parens = parser::peek();
                if (comma_or_closing_parens != "," and comma_or_closing_parens != ")")
                    return make_error_pair("at INSERT INTO: expected comma or closing parens");
                parser::pop();
                if(comma_or_closing_parens == ",") {
                    parser::step = step::insert_values;
                    continue;
                }
                auto curr_insert_row = parser::query.get_current_inserts();
                if(curr_insert_row.size() < parser::query.get_fields_size())
                    return make_error_pair("at INSERT INTO: value count doesn't match field count");
                parser::pop();
                parser::step = step::insert_values_comma_before_opening_parens;
                break;
            }
            case step::insert_values_comma_before_opening_parens: {
                auto comma = parser::peek();
                if(comma != ",")
                    return make_error_pair("at INSERT INTO: expected comma");
                parser::pop();
                parser::step = step::insert_values_opening_parens;
                break;
            }
            default:
                break;
        }
    }
}

