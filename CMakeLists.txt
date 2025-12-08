cmake_minimum_required(VERSION 3.27)
project(DatabaseProject)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

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
FetchContent_MakeAvailable(googletest)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

file(GLOB_RECURSE PRODUCTION_SOURCES CONFIGURE_DEPENDS "engine/*.cpp")

add_library(DatabaseLib STATIC ${PRODUCTION_SOURCES})

target_include_directories(DatabaseLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/backup
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/Dbms
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/executor
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/parser
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/query
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/table
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/utils
)

target_link_libraries(DatabaseLib
        PUBLIC
        Qt6::Widgets
        fmt::fmt
)

add_executable(DatabaseProject main.cpp)

target_link_libraries(DatabaseProject PRIVATE DatabaseLib)

enable_testing()

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "tests/*.cpp")

add_executable(unit_tests ${TEST_SOURCES}
        tests/parser_query_types_test.cpp
        tests/dbms_integration_test.cpp
        tests/dbms_group_by_test.cpp
        tests/dbms_having_test.cpp
        tests/dbms_update_test.cpp)

target_link_libraries(unit_tests PRIVATE
        DatabaseLib
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
