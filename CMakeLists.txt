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
        engine/table/table.hpp
        engine/parser/parser.cpp
        engine/parser/parser.hpp
        engine/query/query.hpp
        engine/parser/structs/parse_error.hpp
        engine/dbms.hpp
        engine/executor/structs/execution_error.hpp
        engine/executor/structs/execution_result.hpp
        engine/executor/executor.cpp
        engine/executor/executor.cpp
        engine/query/structs/condition.hpp
        engine/query/structs/query_operator.hpp
        engine/table/structs/data_type.hpp
        engine/query/complementary/get_operator.hpp
        engine/table/column/column.hpp
        engine/table/row/row.hpp
        engine/utils/ut_print/ut_print.hpp
        engine/backup/recent_change.hpp
        engine/dbms.cpp
        engine/table/column/column.cpp
        engine/table/row/row.cpp
        engine/table/table.cpp
        engine/query/query.cpp
        engine/parser/switch/switch.cpp
        engine/parser/steps/steps.cpp
        engine/parser/complementary/complementary.cpp
        engine/utils/ut_print/ut_print.cpp
        engine/utils/ut_is_identifier/ut_is_identifier.cpp
        engine/utils/ut_is_identifier/ut_is_identifier.hpp
        engine/utils/ut_str_toupper/ut_str_toupper.cpp
        engine/utils/ut_str_toupper/ut_str_toupper.hpp
)
target_link_libraries(DatabaseProject fmt)