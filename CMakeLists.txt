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

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(DatabaseLib STATIC
    engine/parser/Parser.cpp
    engine/query/Query.cpp
    engine/executor/Executor.cpp
    engine/table/Table.cpp
    engine/table/structs/column/Column.cpp
    engine/table/structs/row/Row.cpp
    engine/executor/actions/insert/insert.cpp
    engine/executor/actions/select/select.cpp
    engine/executor/actions/update/update.cpp
    engine/executor/actions/delete/delete_f.cpp
    engine/executor/actions/create/create_table.cpp
    engine/parser/switch/switch.cpp
    engine/parser/steps/steps.cpp
    engine/parser/complementary/complementary.cpp
    engine/utils/ut_print/ut_print.cpp
    engine/utils/ut_is_identifier/ut_is_identifier.cpp
    engine/utils/ut_str_toupper/ut_str_toupper.cpp
    engine/utils/ut_return_join_type/ut_return_join_type.cpp
    engine/Dbms/Dbms.cpp
    engine/Dbms/complementary/load_save.cpp
    engine/Dbms/complementary/make_save.cpp
    engine/Dbms/complementary/execute_query.cpp
    engine/Dbms/complementary/are_consistent.cpp
    engine/parser/steps/step_drop_table.cpp
)
target_include_directories(DatabaseLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/engine)
target_link_libraries(DatabaseLib PRIVATE fmt)

add_executable(DatabaseProject main.cpp)
target_link_libraries(DatabaseProject PRIVATE DatabaseLib)

enable_testing()

add_executable(unit_tests
    tests/test_parser.cpp
)
target_link_libraries(unit_tests PRIVATE DatabaseLib gtest_main fmt)

include(GoogleTest)
gtest_discover_tests(unit_tests)
