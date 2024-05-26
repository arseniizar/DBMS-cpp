//
// Created by Altezza on 06.05.2024.
//

#ifndef DATABASEPROJECT_COLUMN_HPP
#define DATABASEPROJECT_COLUMN_HPP
#include "../row/row.hpp"

struct column {
private:
    std::vector<row> rows;
    std::string col_name;
    data_type type = data_type::UNKNOWN;
public:
    column() = default;

    column(std::vector<row> r, std::string n, data_type t) {
        column::rows = std::move(r);
        column::col_name = std::move(n);
        column::type = t;
    }

    bool empty();

    void set_rows(std::vector<row> r_v);

    data_type get_type();

    std::vector<row> get_rows();

    void remove_row(row &r);

    void insert_row(row row, std::string const &data);

    void set_name(const std::string &name);

    std::string get_name();

    void set_type(data_type t);
};


#endif //DATABASEPROJECT_COLUMN_HPP
