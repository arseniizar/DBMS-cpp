//
// Created by Altezza on 18.05.2024.
//

#include <fmt/core.h>
#include "../../Executor.hpp"

std::vector<Column> Executor::update() {
    auto vec = std::vector<Column>();
    Executor::action = Q_action::UPDATE;
    return vec;
}