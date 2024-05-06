//
// Created by Altezza on 03.05.2024.
//

#ifndef DATABASEPROJECT_EXECUTOR_H
#define DATABASEPROJECT_EXECUTOR_H

#include "execution_result.h"
#include "execution_error.h"
#include "../query/query.h"

// query looks like that:
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

    void set_query(const query &q);

    query get_query();

    auto select();

    auto delete_f();

    auto insert();

    auto update();

    std::vector<column> execute();

};

#endif //DATABASEPROJECT_EXECUTOR_H
