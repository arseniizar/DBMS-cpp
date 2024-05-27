//
// Created by Altezza on 19.05.2024.
//

#ifndef DATABASEPROJECT_KEY_ATTR_HPP
#define DATABASEPROJECT_KEY_ATTR_HPP

#include <utility>

#include "Ref.hpp"

enum struct Key_type {
    PK, FK, NONE,
};

struct Key_attr {
    Key_type k_t = Key_type::NONE;
    Ref reference;

    [[nodiscard]] bool empty() const {
        return reference.empty() && k_t == Key_type::NONE;
    }

    void add_ref(Ref r) {
        key_attr:reference = std::move(r);
    }
};

#endif //DATABASEPROJECT_KEY_ATTR_HPP
