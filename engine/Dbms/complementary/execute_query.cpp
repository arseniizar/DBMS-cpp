#include "../Dbms.hpp"

std::pair<std::vector<Column>, Execution_error> Dbms::execute_query(Query &q) {
    Dbms::executor.set_query(q);
    Dbms::executor.execute();
    Execution_result rec = Dbms::executor.get_execution_res();
    if (Dbms::executor.action == Q_action::SELECT) {
        ut_print(Dbms::execute_select().first);
    } else if (Dbms::executor.action == Q_action::DELETE) {
        Dbms::execute_delete_from();
    } else if (Dbms::executor.action == Q_action::INSERT) {
        Dbms::execute_insert();
    } else if (Dbms::executor.action == Q_action::UPDATE) {
        Dbms::execute_update();
    } else if (Dbms::executor.action == Q_action::CREATE) {
        Dbms::execute_create_table();
    }
    std::string table_name = Dbms::executor.tmp_t.get_table_name();
    Table q_t = *Dbms::find_table_by_name(Dbms::executor.tmp_t.get_table_name());
    q.set_p_table(&q_t);
    auto rec_c = recent_change(&q);
    Dbms::add_rec_change(rec_c);
    Dbms::queries.push_back(q);
    if (!Dbms::executor.get_error().message.empty())
        fmt::println("{}", Dbms::parser.get_error().message);
    else fmt::println("EXECUTED SUCCESSFULLY");
    return std::make_pair(Dbms::executor.tmp_cols, Execution_error());
}

std::pair<std::vector<Column>, Execution_error> Dbms::execute_create_table() {
    auto table_name = Dbms::executor.tmp_t.get_table_name();
    if (is_table_already_exist(table_name)) {
        return make_executor_error("at EXECUTION: Table with name already exists");
    }
    if (!Dbms::executor.q.get_referenced_table_name().empty()) {
        auto are_relations_ok = Dbms::check_relations();
        if (are_relations_ok) Dbms::populate_keys();
    }
    Dbms::add_table(Dbms::executor.tmp_t);
    if (!Dbms::is_loading) ut_print(Dbms::executor.tmp_t);
    return std::make_pair(Dbms::executor.tmp_t.get_columns(), Execution_error());
}

std::pair<std::vector<Column>, Execution_error> Dbms::execute_select() {

}

std::pair<std::vector<Column>, Execution_error> Dbms::execute_delete_from() {

}

std::pair<std::vector<Column>, Execution_error> Dbms::execute_insert() {
    auto table_name = Dbms::executor.tmp_t.get_table_name();
    if (!is_table_already_exist(table_name))
        return make_executor_error("at EXECUTION: unable to insert ot nonexistent table");
    auto field_col_names = std::vector<std::string>();
    for (auto const &insert_field: Dbms::executor.q.get_insert_fields()) {
        field_col_names.push_back(insert_field.value);
    }
    auto col_names = Dbms::find_table_by_name(table_name)->get_columns_names();
    bool is_col_names_consistent =
            std::is_permutation(field_col_names.begin(), field_col_names.end(),
                                col_names.begin(), col_names.end());
    if (!is_col_names_consistent)
        return make_executor_error("at EXECUTION: unable to insert nonexistent columns");
    Dbms::add_and_override_cols(table_name, Dbms::executor.tmp_cols);
    return std::make_pair(Dbms::executor.tmp_cols, Execution_error());
}

std::pair<std::vector<Column>, Execution_error> Dbms::execute_update() {

}