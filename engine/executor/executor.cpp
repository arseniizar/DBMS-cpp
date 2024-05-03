//
// Created by Altezza on 03.05.2024.
//
#include <bits/stdc++.h>
#include "fmt/core.h"
#include "executor.h"
#include "../table.h"

auto executor::set_query(struct query &q) {
    executor::query = q;
}

auto executor::select() {
    auto fields = executor::query.get_fields();
    auto table = *(query.get_table_pointer());
    auto vec = std::vector<column>();
    if (fields.at(0) == "*") {
        for (auto &column: table.get_columns()) vec.push_back(column);
    } else {
        for (auto const &field: fields)
            for (auto &column: table.get_columns())
                if (column.get_name() == field) vec.push_back(column);
    }
    return vec;
}

auto executor::delete_f() {
    return nullptr;
}

auto executor::insert() {
    return nullptr;
}

auto executor::update() {
    return nullptr;
}

void executor::execute() {

}