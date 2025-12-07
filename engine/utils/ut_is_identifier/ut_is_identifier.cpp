//
// Created by Altezza on 17.05.2024.
//

#include "ut_is_identifier.hpp"
#include "../../parser/Parser.hpp"
#include <algorithm>

bool is_identifier(std::string str) {
    std::string upper_str = str;
    std::ranges::transform(upper_str, upper_str.begin(), ::toupper);
    for (auto const &word: reserved_words) {
        if (upper_str == word) {
            return false;
        }
    }

    auto match = std::smatch();
    const auto regex = std::regex("^[a-zA-Z_][a-zA-Z0-9_]*$");
    return std::regex_match(str, match, regex);
}

bool is_identifier_or_asterisk(const std::string &str) {
    return is_identifier(str) || str == "*";
};