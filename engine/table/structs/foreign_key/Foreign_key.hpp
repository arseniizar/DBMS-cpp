//
// Created by Altezza on 26.05.2024.
//

#ifndef DATABASEPROJECT_FOREIGN_KEY_HPP
#define DATABASEPROJECT_FOREIGN_KEY_HPP

#include "../../../query/structs/Key_attr.hpp"
#include "../key/Key.hpp"

struct Foreign_key : Key {
    Foreign_key() = default;
    explicit Foreign_key(std::string const &col_name,
                         std::string const &table_name,
                         Key_attr k_a) {
        Foreign_key::curr_col_name = col_name;
        Foreign_key::curr_table_name = table_name;
        Foreign_key::k_a = std::move(k_a);
    }
};


#endif //DATABASEPROJECT_FOREIGN_KEY_HPP
