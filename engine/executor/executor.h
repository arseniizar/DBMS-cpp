//
// Created by Altezza on 03.05.2024.
//

#ifndef DATABASEPROJECT_EXECUTOR_H
#define DATABASEPROJECT_EXECUTOR_H

#include "execution_result.h"
#include "execution_error.h"
#include "../query.h"

// query looks like that:
// type - I can use inline array of query_type strings
// table_name
// fields
// updates
// aliases
// conditions (where etc.)
// inserts


struct executor {
    query query;
    execution_result result;
    execution_error error;

    void execute();

    auto select();

    auto delete_f();

    auto insert();

    auto update();

    auto set_query(struct query &q);
};

#endif //DATABASEPROJECT_EXECUTOR_H
