//
// Created by Altezza on 26.05.2024.
//

#ifndef DATABASEPROJECT_FOREIGN_KEY_HPP
#define DATABASEPROJECT_FOREIGN_KEY_HPP

#include "../../../query/structs/key_attr.hpp"
#include "../key/key.hpp"

struct foreign_key : key {
    explicit foreign_key(key_attr k_a) {
        foreign_key::k_a = std::move(k_a);
    }
};


#endif //DATABASEPROJECT_FOREIGN_KEY_HPP
