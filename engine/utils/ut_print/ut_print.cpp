#include "../../query/query.hpp"
#include <iomanip>
#include <iostream>

void ut_print(query q) {
    using namespace std;
    cout
            << left
            << setw(14)
            << "Query type"
            << left
            << setw(14)
            << "Join type"
            << left
            << setw(12)
            << "Table Name"
            << left
            << setw(18)
            << "Joined Table Name"
            << endl;

    cout
            << left
            << setw(14)
            << query_type_str[static_cast<int>(q.get_query_type())]
            << left
            << setw(14)
            << join_str[static_cast<int>(q.get_join_type())]
            << left
            << setw(12)
            << q.get_table_name()
            << left
            << setw(18)
            << q.get_joined_table_name()
            << endl;
}

void ut_print() {

}

void ut_print(table t) {

}

void ut_print(std::vector<column> cols) {

}