//
// Created by Altezza on 06.05.2024.
//

#ifndef DATABASEPROJECT_ROW_H
#define DATABASEPROJECT_ROW_H

#include "data_type.h"

struct row {
private:
    std::string data;
    data_type type;
public:
    auto get_data() {
        return row::data;
    }

    auto get_type() {
        return row::type;
    }

    void insert_data(std::string const &ins_data) {
        row::data = ins_data;
    }
};


#endif //DATABASEPROJECT_ROW_H
