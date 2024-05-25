//
// Created by Altezza on 18.05.2024.
//

#include <fmt/core.h>
#include "../../executor.hpp"

std::vector<column> executor::update() {
    auto vec = std::vector<column>();
    executor::action = q_action::UPDATE;
    return vec;
}