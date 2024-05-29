#include "../../query/Query.hpp"
#include <iomanip>
#include <iostream>

void ut_print(Query q) {
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

void ut_print(Table t) {
    using namespace std;
    auto cols = t.get_columns();
    fmt::println("-----------------------");
    cout
            << left
            << setw(18)
            << t.get_table_name() + " columns:"
            << "\n";
    fmt::println("-----------------------");
    std::for_each(cols.begin(), cols.end(), [](Column &col) {
        auto setw_size = col.get_name().size() + 5;
        cout
                << left
                << setw(static_cast<int>(setw_size))
                << col.get_name() + " " + data_types_str[static_cast<int>(col.get_type())]
                << "\n";
    });
    fmt::println("-----------------------");
}

void ut_print(std::vector<Column> cols) {

}