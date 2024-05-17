//
// Created by Altezza on 10.05.2024.
//

#ifndef DATABASEPROJECT_UT_PRINT_HPP
#define DATABASEPROJECT_UT_PRINT_HPP

// so in order to print the columns I selected or deleted or inserted I will
// print them in some kind of table using <iomanip>

#include <iomanip>
#include <vector>
#include "../../table/column/column.hpp"
#include "../../table/table.hpp"

static void print_table(table t) {
    fmt::println("table printed");
};

static void print_columns(std::vector<column> cols) {
    fmt::println("column print");
};


#endif //DATABASEPROJECT_UT_PRINT_HPP
