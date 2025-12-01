//
// Created by Altezza on 11.05.2024.
//

#ifndef DATABASEPROJECT_RECENT_CHANGE_HPP
#define DATABASEPROJECT_RECENT_CHANGE_HPP

#include <string>
#include <iomanip>
#include <sstream>
#include "../table/Table.hpp"
#include "../query/Query.hpp"

struct recent_change {
    std::string time;
    Query* p_query;

    explicit recent_change(Query *p_q) {
        // https://www.tutorialspoint.com/cplusplus/cpp_date_time.htm
        // https://stackoverflow.com/questions/16357999/current-date-and-time-as-string
        recent_change::p_query = p_q;
        time_t now = std::time(0);
        std::tm ltm = *std::localtime(&now);
        std::ostringstream oss;
        oss << std::put_time(&ltm, "%d-%m-%Y %H-%M-%S");
        auto str = oss.str();
        recent_change::time = str;
    }
};

#endif //DATABASEPROJECT_RECENT_CHANGE_HPP
