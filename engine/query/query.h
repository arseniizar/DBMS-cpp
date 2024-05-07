//
// Created by Altezza on 30.04.2024.
//

#ifndef DATABASEPROJECT_QUERY_H
#define DATABASEPROJECT_QUERY_H

#include <string>
#include <vector>
#include <map>
#include "../table/table.h"
#include "condition.h"

enum struct query_type {
    UnknownType,
    Select,
    Update,
    Insert,
    Delete,
    Create
};

inline std::string query_type_str[] = {
        "UnknownType",
        "Select",
        "Update",
        "Insert",
        "Delete",
        "Create"
};

struct query {
private:
    query_type type;
    std::string table_name;
    table* p_table;
    std::map<std::string, data_type> fields;
    std::map<std::string, std::string> updates;
    std::map<std::string, std::string> aliases;
    std::vector<condition> conditions;
    std::vector<std::vector<std::string>> inserts;
public:
    query() = default;

    auto set_p_table(table *p_t) {
        query::p_table = p_t;
    }

    void set_type(query_type type) {
        query::type = type;
    }

    auto get_type() {
        return query::type;
    }

    auto get_conditions() {
        return query::conditions;
    }

    size_t get_fields_size() {
        return query::fields.size();
    }

    void append_field(std::string const &field, data_type const &type) {
        query::fields[field] = type;
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

    auto get_fields() {
        return query::fields;
    }

    auto get_table_name() {
        return query::table_name;
    }

    auto get_table_pointer() {
        return query::p_table;
    }

    auto get_alises() {
        return query::aliases;
    }

    auto get_inserts() {
        return query::inserts;
    }
};


#endif //DATABASEPROJECT_QUERY_H
