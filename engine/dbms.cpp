//
// Created by Altezza on 17.05.2024.
//
#include "dbms.hpp"

void dbms::run() {
    fmt::println("Welcome to the Arsenii`s database!");
    fmt::println("Here you can to write some simple queries!");
    if (dbms::tables.empty())
        fmt::println("It looks like your database is empty, create some table at first");
    fmt::println("If you want to exit, type EXIT :)");
    while (true) {
        auto input = std::string();
        std::getline(std::cin, input);
        if (input == "EXIT") break;
        std::pair<query, parse_error> parse_pair = dbms::parse_query(input);
        if (!parse_pair.second.message.empty()) {
            dbms::parse_errors.push_back(parse_pair.second);
        } else {
            dbms::parser.clean_error();
            std::pair<std::vector<column>, execution_error>
                    execution_pair = dbms::execute_query(parse_pair.first);
            if (!execution_pair.second.message.empty()) {
                dbms::execution_errors.push_back(execution_pair.second);
            } else {
                fmt::println("proceeding...");
            }
        }
    }
    fmt::println("You have exited the program");
}

std::pair<std::vector<column>, execution_error> dbms::execute_query(query &q) {
    dbms::executor.set_query(q);
    dbms::executor.execute();
    execution_result rec = dbms::executor.get_execution_res();
    if (dbms::executor.action == q_action::SELECT) {
        ut_print(dbms::executor.tmp_cols);
    } else if (dbms::executor.action == q_action::DELETE) {
        ut_print(dbms::executor.tmp_cols);
    } else if (dbms::executor.action == q_action::INSERT) {
        ut_print(dbms::executor.tmp_cols);
    } else if (dbms::executor.action == q_action::UPDATE) {
        ut_print(dbms::executor.tmp_cols);
    } else if (dbms::executor.action == q_action::CREATE) {
        ut_print(dbms::executor.tmp_t);
        auto table_name = dbms::executor.tmp_t.get_table_name();
        if (is_table_already_exist(table_name)) {
            return executor_error("at EXECUTION: table with name already exists");
        }
//        if (!dbms::executor.q.get_referenced_table_name().empty()) {
//            dbms::make_relation();
//        }
        dbms::add_table(dbms::executor.tmp_t);
    }
    std::string table_name = dbms::executor.tmp_t.get_table_name();
    table q_t = *dbms::find_table_by_name(dbms::executor.tmp_t.get_table_name());
    q.set_p_table(&q_t);
    auto rec_c = recent_change(&q);
    dbms::add_rec_change(rec_c);
    dbms::queries.push_back(q);
    if (!dbms::executor.get_error().message.empty())
        fmt::println("{}", dbms::parser.get_error().message);
    else fmt::println("EXECUTED SUCCESSFULLY");
    return std::make_pair(dbms::executor.tmp_cols, execution_error());
}

std::pair<query, parse_error> dbms::parse_query(std::string const &str) {
    dbms::parser.input(str);
    query q = dbms::parser.parse();
    if (!dbms::parser.get_error().message.empty())
        fmt::println("{}", dbms::parser.get_error().message);
    else fmt::println("SUCCESSFUL QUERY!");
    dbms::parser.clean();
    return std::make_pair(q, parser.get_error());
}

void dbms::add_table(table t) {
    dbms::table_names.push_back(t.get_table_name());
    dbms::tables.push_back(t);
}

void dbms::add_table(std::string const &name, std::vector<column> const &cols) {
    dbms::table_names.push_back(name);
    dbms::tables.emplace_back(name, cols);
}

void dbms::add_rec_change(const recent_change &rec_c) {
    dbms::recent_changes.push_back(rec_c);
}

std::pair<std::vector<column>, execution_error> dbms::executor_error(std::string const &message) {
    execution_error err = execution_error(message);
    dbms::executor.set_error(err);
    dbms::executor.set_error(err);
    return std::make_pair(dbms::executor.tmp_cols, err);
}

// cool data type, just tried to write it like that :)
__gnu_cxx::__normal_iterator<table *, std::vector<table>>
dbms::find_table_by_name(std::string const &name) {
    return std::find_if(dbms::tables.begin(), dbms::tables.end(),
                        [&name](table t) {
                            return t.get_table_name() == name;
                        });
}

bool dbms::is_table_already_exist(std::string const &table_name) {
    return std::any_of(dbms::table_names.begin(), table_names.end(),
                       [&table_name](std::string const &str) {
                           return str == table_name;
                       });
}

void dbms::print_query(query const &q) {
    ut_print(q);
}

// so in order to make a relation I should check the column for the
// PRIMARY KEY and FOREIGN KEY or inside the CREATE TABLE
bool dbms::check_relations() {
    query q = dbms::executor.q;
    // fields from another table
    auto ref_fields = q.get_referencing_fields_names();
    auto is_referenced_table_exists = dbms::is_table_already_exist
            (q.get_referenced_table_name());
    if (!is_referenced_table_exists) return false;
    auto ref_table = *dbms::find_table_by_name(q.get_referenced_table_name());
    if (!ref_table.contains_cols_names(ref_fields)) return false;
    return true;
}

void dbms::populate_keys() {
    // here I will add all the primary and foreign keys to the table fields
}
