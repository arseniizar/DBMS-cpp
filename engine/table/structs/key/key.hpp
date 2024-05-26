//--+--
// Created by Altezza on 26.05.2024.
//

#ifndef DATABASEPROJECT_KEY_HPP
#define DATABASEPROJECT_KEY_HPP

#include "../column/column.hpp"
#include "../../../query/structs/key_attr.hpp"

struct key {
    key_attr k_a;
    [[nodiscard]] key_attr get_key_attr() const;
};

#endif //DATABASEPROJECT_KEY_HPP
