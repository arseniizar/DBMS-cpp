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
        engine/parser/parse_error.hpp
        engine/dbms.hpp
        engine/executor/execution_error.hpp
        engine/executor/execution_result.hpp
        engine/executor/executor.cpp
        engine/executor/executor.cpp
        engine/query/condition.hpp
        engine/query/query_operator.hpp
        engine/table/data_type.hpp
        engine/query/get_operator.hpp
        engine/table/column.hpp
        engine/table/row.hpp
        engine/utils/print_util.hpp
        engine/backup/recent_change.hpp
        engine/parser/parser_switch.hpp
        engine/parser/parser_complementary.hpp
        engine/parser/parser_steps.hpp
)
target_link_libraries(DatabaseProject fmt)