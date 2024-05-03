//
// Created by Altezza on 30.04.2024.
//

#ifndef DATABASEPROJECT_QUERY_H
#define DATABASEPROJECT_QUERY_H

#include <string>
#include <vector>
#include <map>

enum struct query_type {
    UnknownType,
    Select,
    Update,
    Insert,
    Delete
};

inline std::string query_type_str[] = {
        "UnknownType",
        "Select",
        "Update",
        "Insert",
        "Delete"
};

enum struct query_operator {
    UnknownOperator,
    // =
    eq,
    // !=
    ne,
    // >
    gt,
    // <
    lt,
    // >=
    gte,
    // <=
    lte
};

inline std::string query_operator_str[] = {
        "unknown_operator",
        "eq",
        "ne",
        "gt",
        "lt",
        "gte",
        "lte",
};

struct condition {
    std::string operand1;
    bool operand1_is_field;
    query_operator query_operator;
    std::string operand2;
    bool operand2_is_field;
};

struct query {
private:
    query_type type;
    std::string table_name;
    std::vector<std::string> fields;
    std::map<std::string, std::string> updates;
    std::map<std::string, std::string> aliases;
    std::vector<condition> conditions;
    std::vector<std::vector<std::string>> inserts;
public:
    void set_type(query_type type) {
        query::type = type;
    }

    size_t get_fields_size() {
        return query::fields.size();
    }

    void append_field(std::string const &field) {
        query::fields.push_back(field);
    }

    void append_alias(std::string const& key, std::string const& value) {
        query::aliases[key] = value;
    }

    void append_update(std::string const& key, std::string const& value) {
        query::updates[key] = value;
    }

    void append_condition(const condition& condition) {
        query::conditions.push_back(condition);
    }

    void append_inserts_vec(std::vector<std::string> const& vec) {
        query::inserts.push_back(vec);
    }

    void append_insert(std::string const& insert) {
        query::inserts.back().push_back(insert);
    }

    std::vector<std::string> get_current_inserts() {
        return query::inserts.back();
    }

    struct condition get_current_condition() {
        return query::conditions.back();
    }

    void set_current_condition(condition const& condition) {
        query::conditions.pop_back();
        query::conditions.push_back(condition);
    }

    void set_table_name(const std::string &table_name) {
        query::table_name = table_name;
    }

    std::string to_string() {
        return std::string("query table name: " + query::table_name);
    }
};


#endif //DATABASEPROJECT_QUERY_H
