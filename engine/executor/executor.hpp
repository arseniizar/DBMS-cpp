//
// Created by Altezza on 03.05.2024.
//

#ifndef DATABASEPROJECT_EXECUTOR_HPP
#define DATABASEPROJECT_EXECUTOR_HPP

#include "execution_result.hpp"
#include "execution_error.hpp"
#include "../query/query.hpp"

// q looks like that:
// type - I can use inline array of query_type strings
// table_name
// fields
// updates
// aliases
// conditions (where etc.)
// inserts

enum struct q_action {
    SELECT, DELETE, INSERT, UPDATE, CREATE
};

inline std::string q_action_str[]{
        "SELECT", "DELETE",
        "INSERT", "UPDATE", "CREATE"
};

// I will obtain this from the optimizer as the return type;
struct executable {

};

struct executor {
    query q;
    execution_result result;
    execution_error error;
    std::vector<column> tmp_cols;
    table tmp_t;
    q_action action;

    [[nodiscard]] q_action get_action() const;

    [[nodiscard]] execution_result get_execution_res() const;

    [[nodiscard]] query get_query() const;

    void set_query(const query &que);

    table create_table();

    std::vector<column> select();

    std::vector<column> delete_f();

    std::vector<column> insert();

    std::vector<column> update();

    void execute();

};

#endif //DATABASEPROJECT_EXECUTOR_HPP
