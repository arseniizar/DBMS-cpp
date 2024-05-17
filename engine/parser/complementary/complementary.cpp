//
// Created by Altezza on 17.05.2024.
//
#include <algorithm>
#include "../parser.hpp"
#include "../../utils/ut_is_identifier/ut_is_identifier.hpp"
#include "../../utils/ut_str_toupper/ut_str_toupper.hpp"

bool parser::peek_is_table_name(const std::string &message) {
    auto table_name = parser::peek();
    if (table_name.empty() || table_name == "(") {
        parser::step = step::error;
        parser::error_message = message;
        return false;
    }
    parser::q.set_table_name(table_name);
    parser::pop();
    return true;
}

bool parser::peek_is_operator(std::string const &message) {
    auto op = parser::peek();
    auto curr_cond = parser::q.get_current_condition();
    if (op == "=") curr_cond.query_operator = query_operator::eq;
    else if (op == ">") curr_cond.query_operator = query_operator::gt;
    else if (op == ">=") curr_cond.query_operator = query_operator::gte;
    else if (op == "<") curr_cond.query_operator = query_operator::lt;
    else if (op == "<=") curr_cond.query_operator = query_operator::lte;
    else if (op == "!=") curr_cond.query_operator = query_operator::ne;
    else {
        parser::step = step::error;
        parser::error_message = message;
        return false;
    }
    parser::q.set_current_condition(curr_cond);
    return true;
}

std::pair<query, parse_error>
parser::make_error_pair(std::string const &message) {
    return std::make_pair(parser::q, parse_error(message));
}

std::vector<std::string> parser::split_string_in_words() {
    auto comma = ',';
    auto space = ' ';
    auto words = std::vector<std::string>();
    auto word = std::string();
    for (auto const &c: parser::sql) {
        if (c == comma || c == space) {
            words.push_back(word);
            word.erase();
        }
        word += c;
    }
    words.push_back(word);
    return words;
}

// so I will try to make my own LL(1) parser!
std::string parser::pop() {
    auto pair = parser::peek_with_length();
    if (pair.first.empty()) parser::pop_flag = true;
    parser::index += pair.second;
    parser::pop_whitespace();
    return pair.first;
}

void parser::pop_whitespace() {
    for (;
            parser::index<parser::sql.size() && parser::sql.at(parser::index) == ' ';
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
        if (token == word) return std::make_pair(token, token.size());
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

query parser::parse() {
    auto pair = parser::do_parse();
    parser::error = pair.second;
    return pair.first;
}

// complementary functions for the switch-case functions:
bool parser::peek_is_comma(std::string const &message) {
    auto comma = parser::peek();
    if (comma != ",") {
        parser::step = step::error;
        parser::error_message = message;
        return false;
    }
    parser::pop();
    return true;
}

bool parser::is_index_at_end() {
    if (parser::index >= parser::sql.size() and pop_flag) {
        auto sql_upper = parser::sql;
        str_toupper(sql_upper);
        auto words = split_string_in_words();
        auto bound = words.size();
        auto counter = 0;
        for (auto const &word: words)
            for (auto const &rword: reserved_words)
                if (word == rword) counter++;
        if (counter == bound) {
            parser::step = step::error;
            parser::error_message = "expected a continuation of the statement";
            return false;
        }
        return true;
    }
    return false;
}

bool parser::is_peek_empty() {
    if (parser::peek().empty()) {
        parser::pop_flag = true;
        return true;
    }
    return false;
}