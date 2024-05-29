cmake_minimum_required(VERSION 3.27)
project(DatabaseProject)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG 10.2.1
)

FetchContent_MakeAvailable(fmt)

add_executable(DatabaseProject main.cpp
        engine/table/Table.hpp
        engine/parser/Parser.cpp
        engine/parser/Parser.hpp
        engine/query/Query.hpp
        engine/parser/structs/Parse_error.hpp
        engine/Dbms.hpp
        engine/executor/structs/Execution_error.hpp
        engine/executor/structs/Execution_result.hpp
        engine/executor/Executor.cpp
        engine/executor/Executor.cpp
        engine/query/structs/Condition.hpp
        engine/query/structs/Query_operator.hpp
        engine/table/structs/Data_type.hpp
        engine/query/complementary/get_operator.hpp
        engine/table/structs/column/Column.hpp
        engine/table/structs/row/Row.hpp
        engine/utils/ut_print/ut_print.hpp
        engine/backup/recent_change.hpp
        engine/Dbms.cpp
        engine/table/structs/column/Column.cpp
        engine/table/structs/row/Row.cpp
        engine/table/Table.cpp
        engine/query/Query.cpp
        engine/parser/switch/switch.cpp
        engine/parser/steps/steps.cpp
        engine/parser/complementary/complementary.cpp
        engine/utils/ut_print/ut_print.cpp
        engine/utils/ut_is_identifier/ut_is_identifier.cpp
        engine/utils/ut_is_identifier/ut_is_identifier.hpp
        engine/utils/ut_str_toupper/ut_str_toupper.cpp
        engine/utils/ut_str_toupper/ut_str_toupper.hpp
        engine/utils/ut_return_join_type/ut_return_join_type.cpp
        engine/utils/ut_return_join_type/ut_return_join_type.hpp
        engine/utils/ut_find_in_vector/ut_find_in_vector.hpp
        engine/executor/actions/insert/insert.cpp
        engine/executor/actions/select/select.cpp
        engine/executor/actions/update/update.cpp
        engine/executor/actions/delete/delete_f.cpp
        engine/executor/actions/create/create_table.cpp
        engine/query/structs/Field.hpp
        engine/query/structs/Key_attr.hpp
        engine/query/structs/Ref.hpp
        engine/table/structs/foreign_key/Foreign_key.cpp
        engine/table/structs/foreign_key/Foreign_key.hpp
        engine/table/structs/primary_key/Primary_key.cpp
        engine/table/structs/primary_key/Primary_key.hpp
        engine/table/structs/key/Key.cpp
        engine/table/structs/key/Key.hpp
        engine/parser/steps/step_type.cpp
        engine/parser/steps/step_select.cpp
        engine/parser/steps/step_insert.cpp
        engine/parser/steps/step_update.cpp
        engine/parser/steps/step_delete_from.cpp
        engine/parser/steps/step_where.cpp
        engine/parser/steps/step_create_table.cpp
        engine/parser/steps/step_group_by.cpp
        engine/parser/steps/step_having.cpp
        engine/parser/steps/step_join.cpp
        engine/parser/steps/step_primary_key.cpp
        engine/parser/steps/step_foreign_key.cpp
        engine/parser/steps/step_error.cpp
)
target_link_libraries(DatabaseProject fmt)