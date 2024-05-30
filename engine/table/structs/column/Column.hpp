//
// Created by Altezza on 06.05.2024.
//

#ifndef DATABASEPROJECT_COLUMN_HPP
#define DATABASEPROJECT_COLUMN_HPP
#include "../row/Row.hpp"

struct Column {
private:
    std::vector<Row> rows;
    std::string col_name;
    Data_type type = Data_type::UNKNOWN;
public:
    Column() = default;

    Column(std::vector<Row> r, std::string n, Data_type t) {
        Column::rows = std::move(r);
        Column::col_name = std::move(n);
        Column::type = t;
    }

    bool empty();

    void set_rows(std::vector<Row> r_v);

    Data_type get_type();

    std::vector<Row> get_rows();

    void remove_row(Row &r);

    void add_row(Row row);

    void set_name(const std::string &name);

    std::string get_name();

    void set_type(Data_type t);
};


#endif //DATABASEPROJECT_COLUMN_HPP
