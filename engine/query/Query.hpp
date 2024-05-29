//
// Created by Altezza on 30.04.2024.
//

#ifndef DATABASEPROJECT_QUERY_HPP
#define DATABASEPROJECT_QUERY_HPP

#include <string>
#include <vector>
#include <map>
#include "../table/Table.hpp"
#include "structs/Condition.hpp"
#include "../utils/ut_return_join_type/ut_return_join_type.hpp"
#include "structs/Field.hpp"

enum struct Query_type {
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

struct Query {
private:
    Query_type q_type = Query_type::UnknownType;
    join_type j_type = join_type::UNKNOWN;
    Table* p_table = nullptr;
    std::string command;
    std::string table_name;
    std::string joined_table_name;
    std::vector<std::string> referenced_fields_names;
    std::string referenced_table;
    std::vector<Field> fields;
    std::map<std::string, std::string> updates;
    std::map<std::string, std::string> aliases;
    std::vector<Condition> conditions;
    std::vector<std::vector<std::string>> inserts;
public:
    Query() = default;

    [[nodiscard]] const std::string &get_joined_table_name() const;

    void set_query_type(Query_type q_t);

    void set_join_type(join_type j_t);

    void set_joined_table_name(std::string const &name);

    Query_type get_query_type();

    join_type get_join_type();

    std::vector<Condition> get_conditions();

    size_t get_fields_size();

    void append_field(const Field &f);

    void append_alias(std::string const &key, std::string const &value);

    void append_update(std::string const &key, std::string const &value);

    void append_condition(const Condition &condition);

    void append_inserts_vec(std::vector<std::string> const &vec);

    void append_insert(std::string const &insert);

    std::vector<std::string> get_current_inserts();

    struct Condition get_current_condition();

    void set_current_condition(Condition const &condition);

    void set_table_name(const std::string &name);

    std::vector<Field> get_fields();

    std::string get_table_name();

    std::map<std::string, std::string> get_aliases();

    std::vector<std::vector<std::string>> get_inserts();

    Field get_current_select_field();

    Field get_current_field();

    void set_current_field(Field f);

    size_t get_inserts_size();

    void append_referenced_field_name(std::string const &name);

    std::vector<std::string> get_referenced_fields_names();

    void set_referenced_table(std::string const &name);

    std::string get_referenced_table_name();

    std::vector<Field> get_foreign_keys();

    void set_p_table(Table* p_t);

    Table* get_p_table();

    Field get_primary_key();

    void set_command(std::string const& comm);

    std::string get_command();
};


#endif //DATABASEPROJECT_QUERY_HPP
