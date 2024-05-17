//
// Created by Altezza on 17.05.2024.
//
#include "query.hpp"

void query::set_p_table(table *p_t) {
    query::p_table = p_t;
}

void query::set_join_type(join_type j_t) {
    query::j_type = j_t;
}

join_type query::get_join_type() {
    return query::j_type;
}

void query::set_joined_table_name(std::string const& name) {
    query::joined_table_name = name;
}

void query::set_query_type(query_type q_t) {
    query::q_type = q_t;
}

query_type query::get_query_type() {
    return query::q_type;
}

std::vector<condition> query::get_conditions() {
    return query::conditions;
}

size_t query::get_fields_size() {
    return query::fields.size();
}

void query::append_field(std::string const &field, data_type const &type) {
    query::fields[field] = type;
}

void query::append_alias(std::string const &key, std::string const &value) {
    query::aliases[key] = value;
}

void query::append_update(std::string const &key, std::string const &value) {
    query::updates[key] = value;
}

void query::append_condition(const condition &condition) {
    query::conditions.push_back(condition);
}

void query::append_inserts_vec(std::vector<std::string> const &vec) {
    query::inserts.push_back(vec);
}

void query::append_insert(std::string const &insert) {
    query::inserts.back().push_back(insert);
}

std::vector<std::string> query::get_current_inserts() {
    return query::inserts.back();
}

struct condition query::get_current_condition() {
    return query::conditions.back();
}

void query::set_current_condition(condition const &condition) {
    query::conditions.pop_back();
    query::conditions.push_back(condition);
}

void query::set_table_name(const std::string &name) {
    query::table_name = name;
}

std::map<std::string, data_type> query::get_fields() {
    return query::fields;
}

std::string query::get_table_name() {
    return query::table_name;
}

table *query::get_table_pointer() {
    return query::p_table;
}

std::map<std::string, std::string> query::get_aliases() {
    return query::aliases;
}

std::vector<std::vector<std::string>> query::get_inserts() {
    return query::inserts;
}

std::pair<std::string, data_type> query::get_current_select_field() {
    auto current_select_field = std::pair<std::string, data_type>();
    for (auto i = query::fields.rbegin(); i != fields.rend(); ++i)
        if (i->second == data_type::TABLE_SELECT) return *i;
    return std::make_pair("", data_type::UNKNOWN);
}
