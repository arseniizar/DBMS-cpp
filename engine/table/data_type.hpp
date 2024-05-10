//
// Created by Altezza on 05.05.2024.
//

#ifndef DATABASEPROJECT_DATA_TYPE_HPP
#define DATABASEPROJECT_DATA_TYPE_HPP

#include <string>
#include <regex>

enum struct data_type {
    UNKNOWN, NVARCHAR2, DATE, INTEGER, TABLE_SELECT, INSERT_COL
};

inline std::string data_types_str[]{
        "UNKNOWN", "NVARCHAR2",
        "DATE", "INTEGER",
};

static data_type return_data_type(const std::string &data) {
    auto date_regex = std::regex(R"(\d{1,2}\/\d{1,2}\/\d{2})"),
            integer_regex = std::regex(R"([+-]?(?<!\.)\b[0-9]+\b(?!\.[0-9]))");
    std::vector<std::regex *> regexes = {&date_regex, &integer_regex};
    auto matcher = std::smatch();
    auto count = 1;
    for (auto const &reg: regexes) {
        count++;
        auto matched = std::regex_match(data, matcher, *reg);
        if (matched) {
            return static_cast<data_type>(count);
        }
    }
    return data_type::UNKNOWN;
}

static data_type return_data_type_by_str(std::string const &str) {
    if(str == "NVARCHAR2") return data_type::NVARCHAR2;
    if(str == "DATE") return data_type::DATE;
    if(str == "INTEGER") return data_type::INTEGER;
}

#endif //DATABASEPROJECT_DATA_TYPE_HPP
