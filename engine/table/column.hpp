//
// Created by Altezza on 06.05.2024.
//

#ifndef DATABASEPROJECT_COLUMN_HPP
#define DATABASEPROJECT_COLUMN_HPP

#include <utility>

#include "row.hpp"

struct column {
private:
    std::vector<row> rows;
    std::string col_name;
    data_type type;
public:
    column() = default;
    column(std::vector<row> r, std::string n, data_type t) {
        column::rows = std::move(r);
        column::col_name = std::move(n);
        column::type = t;
    }

    auto get_type() {
        return column::type;
    }

    auto get_rows() {
        return column::rows;
    }

    auto remove_row(row &r) {
        for (auto iter = column::rows.begin(); iter != column::rows.end(); ++iter)
            if (r.get_data() == (*iter).get_data())
                column::rows.erase(iter);
    }

    void insert_row(row row, std::string const &data) {
        row.insert_data(data);
        column::rows.push_back(row);
    }

    void set_name(const std::string &name) {
        column::col_name = name;
    }

    auto get_name() {
        return column::col_name;
    }
};


#endif //DATABASEPROJECT_COLUMN_HPP
