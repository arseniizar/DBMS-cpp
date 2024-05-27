//
// Created by Altezza on 26.05.2024.
//

#ifndef DATABASEPROJECT_FOREIGN_KEY_HPP
#define DATABASEPROJECT_FOREIGN_KEY_HPP

#include "../../../query/structs/Key_attr.hpp"
#include "../key/Key.hpp"

struct Foreign_key : Key {
    explicit Foreign_key(Key_attr k_a) {
        Foreign_key::k_a = std::move(k_a);
    }
};


#endif //DATABASEPROJECT_FOREIGN_KEY_HPP
