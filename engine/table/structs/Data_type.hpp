//
// Created by Altezza on 05.05.2024.
//

#ifndef DATABASEPROJECT_DATA_TYPE_HPP
#define DATABASEPROJECT_DATA_TYPE_HPP

#include <string>
#include <regex>

enum struct Data_type {
    UNKNOWN, NVARCHAR2, DATE, INTEGER, TABLE_SELECT, INSERT_COL
};

inline std::string data_types_str[]{
        "UNKNOWN", "NVARCHAR2",
        "DATE", "INTEGER",
};

static Data_type return_data_type(const std::string &data) {
    auto date_regex = std::regex(R"(\d{1,2}\/\d{1,2}\/\d{2})"),
            integer_regex = std::regex(R"([+-]?(?<!\.)\b[0-9]+\b(?!\.[0-9]))");
    std::vector<std::regex *> regexes = {&date_regex, &integer_regex};
    auto matcher = std::smatch();
    auto count = 1;
    for (auto const &reg: regexes) {
        count++;
        auto matched = std::regex_match(data, matcher, *reg);
        if (matched) {
            return static_cast<Data_type>(count);
        }
    }
    return Data_type::UNKNOWN;
}

static Data_type return_data_type_by_str(std::string const &str) {
    if(str == "NVARCHAR2") return Data_type::NVARCHAR2;
    else if(str == "DATE") return Data_type::DATE;
    else if(str == "INTEGER") return Data_type::INTEGER;
    else return Data_type::UNKNOWN;
}

#endif //DATABASEPROJECT_DATA_TYPE_HPP
