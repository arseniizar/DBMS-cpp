//
// Created by Altezza on 03.05.2024.
//

#ifndef DATABASEPROJECT_DBMS_H
#define DATABASEPROJECT_DBMS_H

#include <vector>
#include "query.h"
#include "table.h"
#include "parser/parser.h"
#include "executor/executor.h"
#include "optimizer/optimizer.h"

struct dbms {
    std::vector<query> queries;
    std::vector<table> tables;
    // in order to undo some devious actions :)
    std::vector<table> recent_backups;
    std::vector<std::string> table_names;
    parser parser;
    executor executor;
    // optimizer/validator
    optimizer optimizer;
};

#endif //DATABASEPROJECT_DBMS_H
