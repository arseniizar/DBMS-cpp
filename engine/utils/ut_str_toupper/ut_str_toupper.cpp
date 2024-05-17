//
// Created by Altezza on 17.05.2024.
//

#include <string>
#include <algorithm>
#include "ut_str_toupper.hpp"

void str_toupper(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}