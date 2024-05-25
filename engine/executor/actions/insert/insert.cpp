//
// Created by Altezza on 18.05.2024.
//

#include "fmt/core.h"
#include "../../executor.hpp"

std::vector<column> executor::insert() {
    auto vec = std::vector<column>();
    executor::action = q_action::INSERT;
    return vec;
}
