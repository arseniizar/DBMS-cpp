//
// Created by Altezza on 03.05.2024.
//

#ifndef DATABASEPROJECT_DBMS_HPP
#define DATABASEPROJECT_DBMS_HPP

#include <vector>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <iostream>
#include "query/query.hpp"
#include "table/table.hpp"
#include "parser/parser.hpp"
#include "executor/executor.hpp"

struct dbms {
private:
    std::vector<query> queries;
    std::vector<table> tables;
    // in order to undo some devious actions :)
    std::vector<table> recent_actions;
    std::vector<std::string> table_names;
    parser parser;
    executor executor;

    auto find_table_by_name(std::string const &name) {
        for (auto &table: tables)
            if (table.get_table_name() == name) return table;
    }

    auto add_rec_change(const table &rec_t) {
        dbms::recent_actions.push_back(rec_t);
    }

    auto parse_query(std::string const &str) {
        dbms::parser.input(str);
        query q = dbms::parser.parse();
        if (!dbms::parser.get_error().message.empty())
            fmt::println("{}", dbms::parser.get_error().message);
        else fmt::println("SUCCESSFUL QUERY!");
        dbms::parser.clean();
        return q;
    }

    auto execute_query(query &q) {
        dbms::executor.set_query(q);
        dbms::executor.execute();
        execution_result rec = dbms::executor.get_execution_res();
        if(dbms::executor.action == q_action::SELECT) {

        } else if (dbms::executor.action == q_action::DELETE) {

        } else if (dbms::executor.action == q_action::INSERT) {

        } else if (dbms::executor.action == q_action::UPDATE) {

        } else if (dbms::executor.action == q_action::CREATE) {

        }
        // table couldn't be found sometimes
        auto table_name = dbms::executor.get_query().get_table_name() + ".rec";
        auto q_t = dbms::find_table_by_name(q.get_table_name());
        q.set_p_table(&q_t);
//        dbms::add_rec_change(table(table_name, *(rec.p_query).);
    }


public:
    auto run() {
        fmt::println("Welcome to the Arsenii`s database!");
        fmt::println("Here you can to write some simple queries!");
        if (dbms::tables.empty())
            fmt::println("It looks like your database is empty, create some table at first");
        fmt::println("If you want to exit, type EXIT :)");
        while (true) {
            auto input = std::string();
            std::getline(std::cin, input);
            if (input == "EXIT") break;
            auto query = dbms::parse_query(input);
            dbms::execute_query(query);
        }
        fmt::println("You have exited the program");
    }
};

#endif //DATABASEPROJECT_DBMS_HPP
