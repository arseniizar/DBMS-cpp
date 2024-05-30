#include "fmt/core.h"
#include "../../Executor.hpp"

std::vector<Column> Executor::insert() {
    auto vec = std::vector<Column>();
    Executor::action = Q_action::INSERT;
    auto insert_fields = Executor::q.get_insert_fields();
    for(auto const& i_f : insert_fields) {
        vec.emplace_back(std::vector<Row>(), i_f.value, i_f.d_t);
    }
    for (auto const &insert_vec: Executor::q.get_inserts()) {
        for(auto const & insert : insert_vec) {
            for(auto &col : vec) {
                if (insert.column_to_insert == col.get_name()) {
                    col.add_row(Row(insert.value, insert.d_t));
                    col.set_type(insert.d_t);
                }
            }
        }
    }
    return vec;
}
