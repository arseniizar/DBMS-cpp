#include <fmt/core.h>
#include <fstream>
#include "../../Executor.hpp"

Table Executor::create_table() {
    Executor::action = Q_action::CREATE;
    Table created_table;
    auto pk = Executor::q.get_primary_key();
    created_table.set_table_name(Executor::q.get_table_name());
    for (auto const &field: Executor::q.get_fields()) {
        Column col = Column(std::vector<Row>(), field.value, field.d_t);
        created_table.insert_column(col);
        if (col.get_name() == pk.value)
            created_table.set_primary_key(
                    Primary_key(col.get_name(), created_table.get_table_name(), pk.k_a));
    }
    return created_table;
}