//
// Created by Altezza on 26.05.2024.
//

#ifndef DATABASEPROJECT_PRIMARY_KEY_HPP
#define DATABASEPROJECT_PRIMARY_KEY_HPP

#include <utility>

#include "../../../query/structs/key_attr.hpp"
#include "../key/key.hpp"

struct primary_key : key {
    explicit primary_key(key_attr k_a) {
        primary_key::k_a = std::move(k_a);
    }
};

#endif //DATABASEPROJECT_PRIMARY_KEY_HPP
