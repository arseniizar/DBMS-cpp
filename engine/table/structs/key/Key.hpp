//--+--
// Created by Altezza on 26.05.2024.
//

#ifndef DATABASEPROJECT_KEY_HPP
#define DATABASEPROJECT_KEY_HPP

#include "../column/Column.hpp"
#include "../../../query/structs/Key_attr.hpp"

struct Key {
    std::string curr_col_name;
    std::string curr_table_name;
    Key_attr k_a;
    [[nodiscard]] Key_attr get_key_attr() const;
};

#endif //DATABASEPROJECT_KEY_HPP
