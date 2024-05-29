//
// Created by Altezza on 03.05.2024.
//

#ifndef DATABASEPROJECT_DBMS_HPP
#define DATABASEPROJECT_DBMS_HPP

#include <vector>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/chrono.h>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include "query/Query.hpp"
#include "table/Table.hpp"
#include "parser/Parser.hpp"
#include "executor/Executor.hpp"
#include "utils/ut_print/ut_print.hpp"
#include "backup/recent_change.hpp"
#include "utils/ut_find_in_vector/ut_find_in_vector.hpp"

struct Dbms {
private:
    std::vector<Query> queries;
    std::vector<Table> tables;
    // for the logs, maybe UNDO in the future
    std::vector<recent_change> recent_changes;
    std::vector<std::string> table_names;
    Parser parser;
    Executor executor;
    // vectors of errors in order to log data etc.
    std::vector<Parse_error> parse_errors;
    std::vector<Execution_error> execution_errors;

    std::pair<std::vector<Column>, Execution_error> executor_error(std::string const &message);

    // cool data type, just tried to write it like that :)
    __gnu_cxx::__normal_iterator<Table *, std::vector<Table>>
    find_table_by_name(std::string const &name);

    bool is_table_already_exist(std::string const &table_name);

    void add_rec_change(const recent_change &rec_c);

    void add_table(std::string const &name, std::vector<Column> const &cols);

    void add_table(Table t);

    std::pair<Query, Parse_error> parse_query(std::string const &str);

    std::pair<std::vector<Column>, Execution_error> execute_query(Query &q);

    bool check_relations();

    void populate_keys();

    void print_query(Query const &q);

    void make_save();

    void load_save();

public:
    void run();
    Dbms();
    ~Dbms();
};

#endif //DATABASEPROJECT_DBMS_HPP
