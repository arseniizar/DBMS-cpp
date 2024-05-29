//
// Created by Altezza on 26.05.2024.
//

#ifndef DATABASEPROJECT_PRIMARY_KEY_HPP
#define DATABASEPROJECT_PRIMARY_KEY_HPP

#include <utility>

#include "../../../query/structs/Key_attr.hpp"
#include "../key/Key.hpp"

struct Primary_key : Key {
    Primary_key() = default;
    explicit Primary_key(std::string const &col_name,
                         std::string const &table_name,
                         Key_attr k_a) {
        Primary_key::curr_col_name = col_name;
        Primary_key::curr_table_name = table_name;
        Primary_key::k_a = std::move(k_a);
    }
};

#endif //DATABASEPROJECT_PRIMARY_KEY_HPP
