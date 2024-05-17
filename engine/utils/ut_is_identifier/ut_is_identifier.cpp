//
// Created by Altezza on 17.05.2024.
//

#include "ut_is_identifier.hpp"
#include "../../parser/parser.hpp"

bool is_identifier(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    for (auto const &word: reserved_words) {
        if (str == word) return false;
    }
    auto match = std::smatch();
    auto regex = std::regex("[a-zA-Z_][a-zA-Z_0-9]*");
    auto matched = std::regex_match(str, match, regex);
    return matched;
}

bool is_identifier_or_asterisk(const std::string &str) {
    return is_identifier(str) || str == "*";
};