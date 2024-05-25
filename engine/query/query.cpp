//
// Created by Altezza on 17.05.2024.
//
#include "query.hpp"

#include <ranges>

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

size_t query::get_inserts_size() {
    return query::inserts.size();
}

void query::append_field(const field& f) {
    query::fields.push_back(f);
}

field query::get_current_field() {
    return query::fields.back();
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

void query::set_current_field(field const &f) {
    query::fields.pop_back();
    query::fields.push_back(f);
}

void query::set_table_name(const std::string &name) {
    query::table_name = name;
}

std::vector<field> query::get_fields() {
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

field query::get_current_select_field() {
    auto current_select_field = std::pair<std::string, data_type>();
    for (auto & field : std::ranges::reverse_view(query::fields))
        if (field.d_t == data_type::TABLE_SELECT) return field;
    return {};
}

void query::append_referencing_field(field *const &p_f) {
    query::referencing_fields.push_back(p_f);
}

void query::set_referencing_fields(std::vector<field *> const &p_fields) {
    query::referencing_fields = p_fields;
}

field * query::get_current_referencing_field() {
    return query::referencing_fields.back();
}

const std::string &query::get_joined_table_name() const {
    return joined_table_name;
}
