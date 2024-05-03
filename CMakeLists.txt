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
        engine/table.h
        engine/parser.cpp
        engine/parser.h
        engine/query.h
        engine/parse_error.h)
target_link_libraries(DatabaseProject fmt)