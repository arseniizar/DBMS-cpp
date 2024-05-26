//
// Created by Altezza on 03.05.2024.
//

#ifndef DATABASEPROJECT_DBMS_HPP
#define DATABASEPROJECT_DBMS_HPP

#include <vector>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <iostream>
#include <algorithm>
#include "query/query.hpp"
#include "table/table.hpp"
#include "parser/parser.hpp"
#include "executor/executor.hpp"
#include "utils/ut_print/ut_print.hpp"
#include "backup/recent_change.hpp"
#include "utils/ut_find_in_vector/ut_find_in_vector.hpp"

struct dbms {
private:
    std::vector<query> queries;
    std::vector<table> tables;
    // for the logs, maybe UNDO in the future
    std::vector<recent_change> recent_changes;
    std::vector<std::string> table_names;
    parser parser;
    executor executor;
    // vectors of errors in order to log data etc.
    std::vector<parse_error> parse_errors;
    std::vector<execution_error> execution_errors;

    std::pair<std::vector<column>, execution_error> executor_error(std::string const &message);

    // cool data type, just tried to write it like that :)
    __gnu_cxx::__normal_iterator<table *, std::vector<table>>
    find_table_by_name(std::string const &name);

    bool is_table_already_exist(std::string const &table_name);

    void add_rec_change(const recent_change &rec_c);

    void add_table(std::string const &name, std::vector<column> const &cols);

    void add_table(table t);

    std::pair<query, parse_error> parse_query(std::string const &str);

    std::pair<std::vector<column>, execution_error> execute_query(query &q);

    bool check_relations();

    void populate_keys();

    void print_query(query const &q);

public:
    void run();

};

#endif //DATABASEPROJECT_DBMS_HPP
