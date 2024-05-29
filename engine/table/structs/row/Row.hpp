//
// Created by Altezza on 06.05.2024.
//

#ifndef DATABASEPROJECT_ROW_HPP
#define DATABASEPROJECT_ROW_HPP

#include "../Data_type.hpp"

struct Row {
private:
    std::string data;
    Data_type type;
public:
    std::string get_data();

    Data_type get_type();

    void set_type(Data_type d_t);

    void insert_data(std::string const &ins_data);
};


#endif //DATABASEPROJECT_ROW_HPP
