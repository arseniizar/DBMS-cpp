//
// Created by Altezza on 17.05.2024.
//
#include <algorithm>
#include "../Parser.hpp"
#include "../../utils/ut_is_identifier/ut_is_identifier.hpp"
#include "../../utils/ut_str_toupper/ut_str_toupper.hpp"

bool Parser::peek_is_table_name(const std::string &message) {
    auto table_name = Parser::peek();
    if (table_name.empty() || table_name == "(") {
        Parser::step = Step::error;
        Parser::error_message = message;
        return false;
    }
    if (Parser::step == Step::join_table) Parser::q.set_joined_table_name(table_name);
    else if (Parser::step == Step::references_table) Parser::q.set_referenced_table(table_name);
    else Parser::q.set_table_name(table_name);
    Parser::pop();
    return true;
}

void Parser::set_additional_step() {
    // group by, where, join
    auto peeked = Parser::peek();
    str_toupper(peeked);
    if (peeked == "WHERE") {
        Parser::step = Step::where;
    } else if (peeked == "JOIN") {
        Parser::step = Step::join;
    } else if (peeked == "GROUP BY") {
        Parser::step = Step::group_by;
    } else {
        Parser::step = Step::error;
        Parser::error_message = "inappropriate continuation of the statement";
    }
}

bool Parser::peek_is_operator(std::string const &message) {
    auto op = Parser::peek();
    auto curr_cond = Parser::q.get_current_condition();
    if (op == "=") curr_cond._operator = Query_operator::eq;
    else if (op == ">") curr_cond._operator = Query_operator::gt;
    else if (op == ">=") curr_cond._operator = Query_operator::gte;
    else if (op == "<") curr_cond._operator = Query_operator::lt;
    else if (op == "<=") curr_cond._operator = Query_operator::lte;
    else if (op == "!=") curr_cond._operator = Query_operator::ne;
    else {
        Parser::step = Step::error;
        Parser::error_message = message;
        return false;
    }
    Parser::q.set_current_condition(curr_cond);
    Parser::pop();
    return true;
}

std::pair<Query, Parse_error>
Parser::make_error_pair(std::string const &message) {
    return std::make_pair(Parser::q, Parse_error(message));
}

std::vector<std::string> Parser::split_string_in_words() {
    auto comma = ',';
    auto space = ' ';
    auto words = std::vector<std::string>();
    auto word = std::string();
    for (auto const &c: Parser::sql) {
        if (c == comma || c == space) {
            words.push_back(word);
            word.erase();
        }
        word += c;
    }
    words.push_back(word);
    return words;
}

// so I will try to make my own LL(1) Parser!
std::string Parser::pop() {
    auto pair = Parser::peek_with_length();
    if (pair.first.empty()) Parser::pop_flag = true;
    Parser::index += pair.second;
    Parser::pop_whitespace();
    return pair.first;
}

void Parser::pop_whitespace() {
    for (;
            Parser::index < Parser::sql.size() && Parser::sql.at(Parser::index) == ' ';
            Parser::index++);
}

std::string Parser::peek() {
    auto peeked = Parser::peek_with_length();
    return peeked.first;
}

std::pair<std::string, int> Parser::peek_with_length() {
    if (Parser::index >= Parser::sql.size()) return std::make_pair("", 0);
    for (auto const &word: reserved_words) {
        auto end_index = std::min(Parser::sql.size(), Parser::index + word.size());
        auto token = Parser::sql.substr(Parser::index, end_index - Parser::index);
        str_toupper(token);
        if (token == word) return std::make_pair(token, token.size());
    }
    // quoted string
    if (Parser::sql[Parser::index] == '\'') {
        return Parser::peek_quoted_with_length();
    }
    // identifier
    return Parser::peek_identifier_with_length();
}

std::pair<std::string, int> Parser::peek_quoted_with_length() {
    if (Parser::sql.size() < Parser::index || Parser::sql.at(index) != '\'')
        return std::make_pair("", 0);
    for (auto i = Parser::index + 1; i < Parser::sql.size(); i++) {
        if (Parser::sql.at(i) == '\'') {
            auto substr = Parser::sql.substr(Parser::index + 1, i - Parser::index - 1);
            return std::make_pair(substr, substr.size() + 2);
        }
    }
    return std::make_pair("", 0);
}

std::pair<std::string, int> Parser::peek_identifier_with_length() {
    for (auto i = Parser::index; i < Parser::sql.size(); ++i) {
        auto regex = std::regex("[a-zA-Z0-9_*]");
        auto match = std::smatch();
        auto str = std::string(1, Parser::sql.at(i));
        auto matched = std::regex_match(str, match, regex);
        if (!matched) {
            auto substr = Parser::sql.substr(Parser::index, i - Parser::index);
            return std::make_pair(substr, substr.size());
        }
    }
    auto substr = Parser::sql.substr(Parser::index);
    return std::make_pair(substr, substr.size());
}

Query Parser::parse() {
    auto pair = Parser::do_parse();
    Parser::error = pair.second;
    return pair.first;
}

// complementary functions for the switch-case functions:
bool Parser::peek_is_comma(std::string const &message) {
    auto comma = Parser::peek();
    if (comma != ",") {
        Parser::step = Step::error;
        Parser::error_message = message;
        return false;
    }
    Parser::pop();
    return true;
}

bool Parser::is_index_at_end() {
    if (Parser::index >= Parser::sql.size() and pop_flag) {
        auto sql_upper = Parser::sql;
        str_toupper(sql_upper);
        auto words = split_string_in_words();
        auto bound = words.size();
        auto counter = 0;
        for (auto const &word: words)
            for (auto const &rword: reserved_words)
                if (word == rword) counter++;
        if (counter == bound) {
            Parser::step = Step::error;
            Parser::error_message = "expected a continuation of the statement";
            return false;
        }
        return true;
    }
    return false;
}

bool Parser::is_peek_empty() {
    if (Parser::peek().empty()) {
        Parser::pop_flag = true;
        return true;
    }
    return false;
}

__gnu_cxx::__normal_iterator<Field *, std::vector<Field>>
Parser::get_field_by_name(const std::string &name) {
    return std::ranges::find_if(
            Parser::q.get_fields().begin(),
            Parser::q.get_fields().end(),
            [&name](Field const &f) {
                return f.value == name;
            }
    );
}

bool Parser::peek_is_opening_parens(const std::string &message) {
    auto opening_parens = Parser::peek();
    if (opening_parens != "(") {
        Parser::step = Step::error;
        Parser::error_message = message;
        return false;
    }
    Parser::pop();
    return true;
}