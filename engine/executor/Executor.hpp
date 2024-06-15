
#ifndef DATABASEPROJECT_EXECUTOR_HPP
#define DATABASEPROJECT_EXECUTOR_HPP

#include "structs/Execution_result.hpp"
#include "structs/Execution_error.hpp"
#include "../query/Query.hpp"

// q looks like that:
// type - I can use inline array of Query_type strings
// table_name
// fields
// updates
// aliases
// conditions (where etc.)
// inserts

enum struct Q_action {
    SELECT, DELETE, INSERT, UPDATE, CREATE
};

inline std::string q_action_str[]{
        "SELECT", "DELETE",
        "INSERT", "UPDATE", "CREATE"
};

struct Executor {
    Query q;
    Execution_result result;
    Execution_error error;
    std::vector<Column> tmp_cols;
    Table tmp_t;
    Q_action action;

    [[nodiscard]] Q_action get_action() const;

    [[nodiscard]] Execution_result get_execution_res() const;

    [[nodiscard]] Execution_error get_error() const;

    [[nodiscard]] Query get_query() const;

    void set_error(Execution_error const &err);

    void set_query(const Query &que);

    Table create_table();

    std::vector<Column> delete_f();

    std::vector<Column> insert();

    std::vector<Column> update();

    std::vector<Column> select();

    void execute();

    void set_tmp_t(Table t);

    void clean_error();
};

#endif //DATABASEPROJECT_EXECUTOR_HPP
