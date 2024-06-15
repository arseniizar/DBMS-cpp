#include "../../query/Query.hpp"
#include <iomanip>
#include <iostream>

void print_line(int length) {
    for (auto i = 0; i < length; i++)
        fmt::print("-");
    fmt::println("");
}

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
    fmt::println("-------------------------------------------------------------------");
    cout
            << left
            << setw(18)
            << t.get_table_name() + " columns:"
            << "\n";
    fmt::println("-------------------------------------------------------------------");
    std::for_each(cols.begin(), cols.end(), [](Column &col) {
        auto setw_size = col.get_name().size() + 5;
        cout
                << left
                << setw(static_cast<int>(setw_size))
                << col.get_name() + " " + data_types_str[static_cast<int>(col.get_type())]
                << "\n";
    });
    fmt::println("-------------------------------------------------------------------");
}

void ut_print(std::vector<Column> cols) {
    using namespace std;
    size_t rows_length = 0;
    for (auto &col: cols)
        for (auto &row: col.get_rows())
            rows_length += 15;
    auto line_length = 30 + 25 + rows_length;
    print_line(line_length);
    cout
            << left
            << setw(30)
            << "Columns:"
            << left
            << setw(25)
            << "Types:"
            << left
            << setw(25)
            << "Rows:"
            << "\n";
    print_line(line_length);
    std::for_each(cols.begin(), cols.end(), [](Column &col) {
        auto setw_size = col.get_name().size() + 5;
        auto rows = col.get_rows();
        cout
                << left
                << setw(30)
                << col.get_name() + " "
                << setw(25)
                << data_types_str[static_cast<int>(col.get_type())];
        std::for_each(rows.begin(), rows.end(), [](Row &row) {
            auto setw_size = row.get_data().size() + 5;
            cout
                    << left
                    << setw(15)
                    << row.get_data()
                    << " ";
        });
        cout << "\n";
    });
    print_line(line_length);
}