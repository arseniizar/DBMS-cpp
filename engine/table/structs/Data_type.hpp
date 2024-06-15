//
// Created by Altezza on 05.05.2024.
//

#ifndef DATABASEPROJECT_DATA_TYPE_HPP
#define DATABASEPROJECT_DATA_TYPE_HPP

#include <string>
#include <regex>
#include "../../utils/ut_str_toupper/ut_str_toupper.hpp"

enum struct Data_type {
    UNKNOWN, NVARCHAR2, DATE, INTEGER, NONE, TABLE_SELECT, INSERT_COL, UPDATE_COL
};

inline std::string data_types_str[]{
        "UNKNOWN", "NVARCHAR2",
        "DATE", "INTEGER", "NONE"
};

static Data_type return_data_type(const std::string &data) {
    auto upper_data = data;
    str_toupper(upper_data);
    // https://www.mtu.edu/umc/services/websites/writing/characters-avoid/
    auto restricted_chars = std::vector<char>{
            '\'', '*', ':', '/', '\\', '?', '"', '`', '~', '&', '$', '|', '<', '>', '^', '#'
    };
    if (upper_data == "NONE")
        return Data_type::NONE;
    if (!data.empty() and std::all_of(data.begin(), data.end(), ::isdigit))
        return Data_type::INTEGER;
    for (auto const &r_c: restricted_chars) {
        if (!data.empty() and std::all_of(data.begin(), data.end(),
                                          [&r_c](char const &c) {
                                              return c != r_c;
                                          }))
            return Data_type::NVARCHAR2;
    }
    return Data_type::UNKNOWN;
}

static Data_type return_data_type_by_str(std::string const &str) {
    if (str == "NVARCHAR2") return Data_type::NVARCHAR2;
    else if (str == "DATE") return Data_type::DATE;
    else if (str == "INTEGER") return Data_type::INTEGER;
    else if (str == "NONE") return Data_type::NONE;
    else return Data_type::UNKNOWN;
}

#endif //DATABASEPROJECT_DATA_TYPE_HPP
