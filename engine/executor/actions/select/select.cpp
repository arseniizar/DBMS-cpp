#include "../../Executor.hpp"

std::vector<Column> Executor::select() {
    auto fields = Executor::q.get_fields();
    auto query = Executor::q;
    auto vec = std::vector<Column>();
    for(auto const &field : fields) {
        vec.emplace_back(std::vector<Row>(), field.value, Data_type::TABLE_SELECT);
    }
    Executor::action = Q_action::SELECT;
    return vec;
}