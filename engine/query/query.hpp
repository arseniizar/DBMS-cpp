//
// Created by Altezza on 30.04.2024.
//

#ifndef DATABASEPROJECT_QUERY_HPP
#define DATABASEPROJECT_QUERY_HPP

#include <string>
#include <vector>
#include <map>
#include "../table/table.hpp"
#include "structs/condition.hpp"

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
    table *p_table;
    std::map<std::string, data_type> fields;
    std::map<std::string, std::string> updates;
    std::map<std::string, std::string> aliases;
    std::vector<condition> conditions;
    std::vector<std::vector<std::string>> inserts;
public:
    query() = default;

    void set_p_table(table *p_t);

    void set_type(query_type t);

    query_type get_type();

    std::vector<condition> get_conditions();

    size_t get_fields_size();

    void append_field(std::string const &field, data_type const &type);

    void append_alias(std::string const &key, std::string const &value);

    void append_update(std::string const &key, std::string const &value);

    void append_condition(const condition &condition);

    void append_inserts_vec(std::vector<std::string> const &vec);

    void append_insert(std::string const &insert);

    std::vector<std::string> get_current_inserts();

    struct condition get_current_condition();

    void set_current_condition(condition const &condition);
    void set_table_name(const std::string &name);

    std::map<std::string, data_type> get_fields();

    std::string get_table_name();

    table* get_table_pointer();

    std::map<std::string, std::string> get_aliases();

    std::vector<std::vector<std::string>> get_inserts();

    std::pair<std::string, data_type> get_current_select_field();
};


#endif //DATABASEPROJECT_QUERY_HPP
