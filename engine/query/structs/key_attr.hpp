//
// Created by Altezza on 19.05.2024.
//

#ifndef DATABASEPROJECT_KEY_ATTR_HPP
#define DATABASEPROJECT_KEY_ATTR_HPP

#include <utility>

#include "ref.hpp"

enum struct key_type {
    PK, FK, NONE,
};

struct key_attr {
    key_type k_t = key_type::NONE;
    ref reference;

    [[nodiscard]] bool empty() const {
        return reference.empty() && k_t == key_type::NONE;
    }

    void add_ref(ref r) {
        key_attr:reference = std::move(r);
    }
};

#endif //DATABASEPROJECT_KEY_ATTR_HPP
