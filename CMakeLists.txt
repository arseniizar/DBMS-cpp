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

file(GLOB_RECURSE ENGINE_SOURCES
    "engine/Dbms/*.cpp"
    "engine/parser/*.cpp"
    "engine/executor/*.cpp"
    "engine/query/*.cpp"
    "engine/table/*.cpp"
    "engine/utils/*.cpp"
    "engine/backup/*.cpp"
)

add_library(dbms_engine STATIC ${ENGINE_SOURCES})

target_include_directories(dbms_engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(dbms_engine PUBLIC fmt::fmt)


file(GLOB UI_SOURCES "engine/ui/*.cpp")

set(RESOURCE_FILE resources/resources.qrc
        engine/ui/codeeditor.cpp
        engine/ui/codeeditor.hpp
        engine/ui/querytab.cpp
        engine/ui/querytab.hpp
        engine/ui/helpwidget.cpp
        engine/ui/helpwidget.hpp)

add_executable(
        dbms_app main.cpp
        ${UI_SOURCES}
        ${RESOURCE_FILE}
)

target_link_libraries(dbms_app PRIVATE
    dbms_engine
    Qt6::Widgets
)


enable_testing()

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "tests/*.cpp")

add_executable(unit_tests ${TEST_SOURCES})

target_link_libraries(unit_tests PRIVATE
    dbms_engine
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)

set_target_properties(dbms_app PROPERTIES OUTPUT_NAME DatabaseProject)
