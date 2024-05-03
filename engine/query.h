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
    Eq,
    // !=
    Ne,
    // >
    Gt,
    // <
    Lt,
    // >=
    Gte,
    // <=
    Lte
};

inline std::string query_operator_str[] = {
        "UnknownOperator",
        "Eq",
        "Ne",
        "Gt",
        "Lt",
        "Gte",
        "Lte",
};

enum struct condition {
    Operand1,
    OperandIsField,
    Operator,
    Operand2,
    Operand2IsField
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

    void append_field(std::string const &field) {
        query::fields.push_back(field);
    }

    void append_alias(std::string const& key, std::string const& value) {
        query::aliases[key] = value;
    }

    void set_table_name(const std::string &table_name) {
        query::table_name = table_name;
    }
};


#endif //DATABASEPROJECT_QUERY_H
