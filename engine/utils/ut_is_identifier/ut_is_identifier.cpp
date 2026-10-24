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

    if (str.empty()) return false;
    const auto first = static_cast<unsigned char>(str[0]);
    if (!std::isalpha(first) && first != '_') return false;
    for (size_t i = 1; i < str.size(); ++i) {
        const auto ch = static_cast<unsigned char>(str[i]);
        if (!std::isalnum(ch) && ch != '_') return false;
    }
    return true;
}

bool is_identifier_or_asterisk(const std::string &str) {
    return is_identifier(str) || str == "*";
};