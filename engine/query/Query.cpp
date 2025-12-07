//
// Created by Altezza on 17.05.2024.
//
#include "Query.hpp"
#include <ranges>

void Query::set_join_type(join_type j_t) {
    Query::j_type = j_t;
}

join_type Query::get_join_type() {
    return Query::j_type;
}

void Query::set_joined_table_name(std::string const& name) {
    Query::joined_table_name = name;
}

void Query::set_query_type(Query_type q_t) {
    Query::q_type = q_t;
}

Query_type Query::get_query_type() {
    return Query::q_type;
}

std::vector<Condition> Query::get_conditions() {
    return Query::conditions;
}

size_t Query::get_fields_size() {
    return Query::fields.size();
}

size_t Query::get_inserts_size() {
    return Query::inserts.size();
}

void Query::append_field(const Field& f) {
    Query::fields.push_back(f);
}

Field Query::get_current_field() {
    return Query::fields.back();
}

void Query::append_alias(std::string const& key, std::string const& value) {
    Query::aliases[key] = value;
}

void Query::append_update(std::string const& key, std::string const& value) {
    //    Query::updates[key] = value;
}

void Query::append_condition(const Condition& condition) {
    Query::conditions.push_back(condition);
}

void Query::append_inserts_vec(std::vector<Insert> const& vec) {
    Query::inserts.push_back(vec);
}

void Query::append_insert(Insert const& insert) {
    Query::inserts.back().push_back(insert);
}

std::vector<Insert> Query::get_current_inserts() {
    return Query::inserts.back();
}

struct Condition Query::get_current_condition() {
    return Query::conditions.back();
}

void Query::set_current_condition(Condition const& condition) {
    Query::conditions.pop_back();
    Query::conditions.push_back(condition);
}

void Query::set_current_field(Field f) {
    Query::fields.pop_back();
    Query::fields.push_back(f);
}

void Query::set_table_name(const std::string& name) {
    Query::table_name = name;
}

std::vector<Field> Query::get_fields() {
    return Query::fields;
}

std::string Query::get_table_name() {
    return Query::table_name;
}

std::map<std::string, std::string> Query::get_aliases() {
    return Query::aliases;
}

std::vector<std::vector<Insert>> Query::get_inserts() {
    return Query::inserts;
}

Field Query::get_current_select_field() {
    auto current_select_field = std::pair<std::string, Data_type>();
    for (auto& field : std::ranges::reverse_view(Query::fields))
        if (field.d_t == Data_type::TABLE_SELECT) return field;
    return {};
}

const std::string& Query::get_joined_table_name() const {
    return joined_table_name;
}

void Query::append_referenced_field_name(const std::string& name) {
    Query::referenced_fields_names.push_back(name);
}

std::vector<std::string> Query::get_referenced_fields_names() {
    return Query::referenced_fields_names;
}

void Query::set_referenced_table(const std::string& name) {
    Query::referenced_table = name;
}

std::string Query::get_referenced_table_name() {
    return Query::referenced_table;
}

Field Query::get_primary_key() {
    Field pk;
    std::for_each(Query::fields.begin(), Query::fields.end(),
                  [&pk](Field& f) {
                      if (f.k_a.k_t == Key_type::PK) pk = f;
                  });
    return pk;
}

std::vector<Field> Query::get_foreign_keys() {
    std::vector<Field> foreign_keys;
    std::for_each(Query::fields.begin(), Query::fields.end(),
                  [&foreign_keys](Field& f) {
                      if (f.k_a.k_t == Key_type::FK) foreign_keys.push_back(f);
                  });
    return foreign_keys;
}

Table* Query::get_p_table() {
    return Query::p_table;
}

void Query::set_p_table(Table* p_t) {
    Query::p_table = p_t;
}

void Query::set_command(const std::string& comm) {
    Query::command = comm;
}

std::string Query::get_command() {
    return Query::command;
}

std::vector<Field> Query::get_insert_fields() const {
    auto vec = std::vector<Field>();
    for (auto& field : Query::fields)
        if (field.d_t == Data_type::INSERT_COL)
            vec.push_back(field);
    return vec;
}

Field Query::get_next_insert_field() {
    if (const auto insert_fields = Query::get_insert_fields(); insert_fields.empty()) return {};

    if (Query::insert_field_count < Query::get_insert_fields().size())
        return Query::get_insert_fields()[Query::insert_field_count++];
    else {
        Query::insert_field_count = 0;
        return Query::get_insert_fields()[Query::insert_field_count++];
    }
}

Field Query::find_field_by_value(const std::string& value) {
    for (auto const& field : Query::fields) {
        if (field.value == value) return field;
    }
    return {};
}

void Query::set_current_update(Update const& u) {
    Query::updates.pop_back();
    Query::updates.push_back(u);
}
