#include "../Dbms.hpp"
#include "../../utils/ut_contains_elems/ut_contains_elems.hpp"

// check whether conditions from the query and the columns from the execution are a permutation of the
// columns from the table found by the table_name from query
std::pair<std::vector<Column>, Execution_error> Dbms::are_consistent() {
    auto pair1 = are_cols_consistent();
    auto pair2 = are_conditions_consistent();
    if (pair1.second.message.empty() and pair2.second.message.empty()) {
        return make_executor_error("");
    } else if (pair1.second.message.empty() and !pair2.second.message.empty()) {
        return make_executor_error(pair2.second.message);
    } else if (!pair1.second.message.empty() and pair2.second.message.empty()) {
        return make_executor_error(pair1.second.message);
    } else if (!pair1.second.message.empty() and !pair2.second.message.empty()) {
        return make_executor_error("at EXECUTION: both conditions and columns are inconsistent");
    }
    return make_executor_error("");
}

std::pair<std::vector<Column>, Execution_error> Dbms::are_cols_consistent() {
    auto table_name = Dbms::executor.q.get_table_name();
    auto field_col_names = std::vector<std::string>();
    if (!Dbms::executor.q.get_insert_fields().empty())
        for (auto const &insert_field: Dbms::executor.q.get_insert_fields()) {
            field_col_names.push_back(insert_field.value);
        }
    else if (Dbms::executor.q.get_query_type() == Query_type::Delete) {
        if (!is_table_already_exist(table_name)) {
            Execution_error exe_er = Execution_error(
                    "at EXECUTION: it is not possible to delete from nonexistent tables");
            Dbms::executor.error = exe_er;
            return make_executor_error(exe_er.message);
        }
        return make_executor_error("");
    }
    // case for the * in select
    auto fields = Dbms::executor.q.get_fields();
    if (fields.at(0).value == "*" and fields.size() == 1) {
        return make_executor_error("");
    }
    auto col_names = Dbms::find_table_by_name(table_name).get_columns_names();
    // in case of adding less than all columns
    if (field_col_names.size() < col_names.size() and Dbms::executor.action != Q_action::SELECT) {
        Execution_error exe_er = Dbms::add_and_override_cols(table_name, Dbms::executor.tmp_cols);
        if (!exe_er.message.empty()) {
            Dbms::executor.error = exe_er;
            return make_executor_error(exe_er.message);
        }
        return std::make_pair(Dbms::executor.tmp_cols, Execution_error());
    }

    bool are_col_names_consistent =
            ut_contains_elems(col_names, field_col_names);
    if (!are_col_names_consistent) {
        std::string message = "at EXECUTION: unable to access nonexistent columns";
        Dbms::executor.error = Execution_error(message);
        return make_executor_error(message);
    }
    return make_executor_error("");
}

std::pair<std::vector<Column>, Execution_error> Dbms::are_conditions_consistent() {
    auto table = Dbms::find_table_by_name(Dbms::executor.q.get_table_name());
    if (Dbms::executor.q.get_conditions().empty()) {
        return make_executor_error("");
    }
    auto condition_col_names = std::vector<std::string>();
    for (auto &condition: Dbms::executor.q.get_conditions()) {
        if (condition.operand1_is_field)
            condition_col_names.push_back(condition.operand1);
        else if (condition.operand2_is_field)
            condition_col_names.push_back(condition.operand2);
        else {
            std::string message = "at EXECUTION: improper condition for WHERE";
            Dbms::executor.error = Execution_error(message);
            return make_executor_error(message);
        }
    }
    bool are_col_names_consistent = ut_contains_elems(
            Dbms::find_table_by_name(Dbms::executor.q.get_table_name()).get_columns_names(),
            condition_col_names
    );
    if (!are_col_names_consistent) {
        std::string message = "at EXECUTION: unable to access nonexistent columns";
        Dbms::executor.error = Execution_error(message);
        return make_executor_error(message);
    }
//    std::vector<Data_type> conditions_d_t;
//    std::vector<Data_type> cols_d_t;
//    for (auto &condition: Dbms::executor.q.get_conditions()) conditions_d_t.push_back(condition.d_t);
//    for (auto &col: table.get_columns()) conditions_d_t.push_back(col.get_type());
//    bool are_types_consistent = ut_contains_elems(
//            cols_d_t,
//            conditions_d_t
//    );
//    if (!are_types_consistent) {
//        std::string message = "at EXECUTION: improper condition for WHERE (improper operator for that type of data)";
//        Dbms::executor.error = Execution_error(message);
//        return make_executor_error(message);
//    }
    return make_executor_error("");
}

