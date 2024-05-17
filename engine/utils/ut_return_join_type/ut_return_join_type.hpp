//
// Created by Altezza on 17.05.2024.
//

#ifndef DATABASEPROJECT_UT_RETURN_JOIN_TYPE_HPP
#define DATABASEPROJECT_UT_RETURN_JOIN_TYPE_HPP


#include <vector>
#include <string>

inline std::vector<std::string> joins{
        "JOIN", "LEFT JOIN", "RIGHT JOIN", "FULL JOIN", "UNKNOWN"
};

enum struct join_type {
    JOIN, LEFT_JOIN, RIGHT_JOIN, FULL_JOIN, UNKNOWN
};

std::string return_join_type_str(std::string const &join);

join_type return_join_type(std::string const &join);

#endif //DATABASEPROJECT_UT_RETURN_JOIN_TYPE_HPP
