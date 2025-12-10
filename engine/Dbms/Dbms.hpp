//
// Created by Altezza on 03.05.2024.
//

#ifndef DATABASEPROJECT_DBMS_HPP
#define DATABASEPROJECT_DBMS_HPP

#include <vector>
#include "fmt/core.h"
#include "fmt/format.h"
#include <filesystem>
#include <sstream>
#include "../query/Query.hpp"
#include "../table/Table.hpp"
#include "../parser/Parser.hpp"
#include "../executor/Executor.hpp"
#include "../backup/recent_change.hpp"
#include <variant>

using QueryResult = std::variant<std::string, std::vector<Column>>;

struct Dbms {
private:
    std::vector<Query> queries;
    std::vector<Table> tables;
    std::string table_saves_path = R"(..\engine\saves\tables\)";
    // for the logs, maybe UNDO in the future
    std::vector<recent_change> recent_changes;
    std::vector<std::string> table_names;
    Parser parser;
    Executor executor;
    // vectors of errors in order to log data etc.
    std::vector<Parse_error> parse_errors;
    std::vector<Execution_error> execution_errors;
    bool is_dbms_changed;
    bool is_loading;
    std::string error_message;

    std::pair<std::vector<Column>, Execution_error> make_executor_error(std::string const& message);

    Table find_table_by_name(std::string const& name);

    bool is_table_already_exist(std::string const& table_name);

    void add_rec_change(const recent_change& rec_c);

    void add_table(std::string const& name, std::vector<Column> const& cols);

    void add_table(Table t);

    std::pair<Query, Parse_error> parse_query(std::string const& str);

    void update_recent_change(Query& q);
    std::pair<std::vector<Column>, Execution_error> execute_query(Query& q);

    bool check_relations();

    void populate_keys();

    std::vector<Query> get_create_queries();

    std::vector<Query> get_insert_queries();

    void make_save();

    void load_save();

    void load_save_menu();

    void load_prompt();

    Execution_error add_and_override_cols(std::string const& table_name, std::vector<Column> cols);

    std::pair<std::vector<Column>, Execution_error> execute_create_table();

    std::pair<std::vector<Column>, Execution_error> execute_select();

    std::pair<std::vector<Column>, Execution_error> execute_delete_from();

    std::pair<std::vector<Column>, Execution_error> execute_insert();

    std::pair<std::vector<Column>, Execution_error> execute_update();

    std::vector<Column> execute_group_by_with_having(std::vector<Column>& input_cols,
                                                     const std::vector<std::string>& group_by_cols_names,
                                                     const std::vector<Field>& requested_fields,
                                                     const std::vector<Condition>& having_conditions);

    std::pair<std::vector<Column>, Execution_error> are_consistent();

    std::pair<std::vector<Column>, Execution_error> are_cols_consistent();

    std::pair<std::vector<Column>, Execution_error> are_conditions_consistent();

    std::pair<std::vector<Column>, Execution_error> drop_table(std::string const& table_name);

public:
    void print_table_names();

    std::string process_query_to_string(const std::string& input);

    QueryResult process_query(const std::string& input);

    Dbms();

    ~Dbms();
};

#endif //DATABASEPROJECT_DBMS_HPP
