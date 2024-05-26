//
// Created by Altezza on 06.05.2024.
//

#ifndef DATABASEPROJECT_ROW_HPP
#define DATABASEPROJECT_ROW_HPP

#include "../data_type.hpp"

struct row {
private:
    std::string data;
    // maybe I do not need the type right now, maybe I will need it after some time
    // for example for inserting
     data_type type;
public:
    std::string get_data();

    data_type get_type();

    void insert_data(std::string const &ins_data);
};


#endif //DATABASEPROJECT_ROW_HPP
